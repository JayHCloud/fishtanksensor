#include "mqtt.h"
#include "ledconfig.h"
#include "watertemp.h"

#define BROKER_URL SERVER_IP // Format: mqtt://XXX.XXX.X.XXX:1883



static const char *TAG = "MQTT";


static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
Event handler that istens for commands from server
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;


    
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        //tells server that the device is on and connected
        msg_id = esp_mqtt_client_publish(client, "/ds18s20/state", "On", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        //topic for light toggle commands
        msg_id = esp_mqtt_client_subscribe(client, "/ds18s20/light", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
    
        //topic to send temperature to server
        msg_id = esp_mqtt_client_subscribe(client, "/ds18s20/temp", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;



    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);

        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");

        if (strncmp(event->topic, "/ds18s20/light", event->topic_len) == 0) { //if topic = /ds18s20/light
            toggle_led();  // toggles LED on command
            break;
    } 
        else if (strncmp(event->topic, "/ds18s20/temp ", event->topic_len) == 0) { //if topic = /ds18s20/temp
            char floatStr[6];   //6 chars to fit temperature and null "78.25/0"
            float currentTemp = 0;
            currentTemp = ds18b20_get_temp();
            snprintf(floatStr, sizeof(floatStr), "%.2f", currentTemp); // %.2f for two decimal places
            floatStr[sizeof(floatStr) - 1] = '\0'; // Ensure null-termination
            msg_id = esp_mqtt_client_publish(client, "/ds18s20/tempS", floatStr, 0, 0, 0); //sends to subtopic /tempS
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;

    }  
         else {
            break;

    }
    break;

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URL,
    };


    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);   
    esp_mqtt_client_start(client);
}


