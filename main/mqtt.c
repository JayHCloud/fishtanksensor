#include "include/mqtt.h"

#define BROKER_URL SERVER_IP //Example: mqtt://192.168.1.142:1883

static const char *TAG = "MQTT";

esp_mqtt_client_handle_t client;



static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URL,
    };


    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);   
    esp_mqtt_client_start(client);
}




void sendMqttTemp(char *str){ //make this function more generic, for different topics or..
    int msg_id = esp_mqtt_client_publish(client, "/tankesp32/temp", str, 0, 0, 0); 
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

}


//Event handler that istens for commands from server

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
        msg_id = esp_mqtt_client_publish(client, "/tankesp32/state", "On", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        //sub to topic for light toggle commands
        msg_id = esp_mqtt_client_subscribe(client, "/tankesp32/light", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
    
        //sub totopic to send temperature to server
        msg_id = esp_mqtt_client_subscribe(client, "/tankesp32/temp", 0);
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
            //If topic is /tankesp32/temp and message is getTemp
         if ((strncmp(event->topic, "/tankesp32/temp ", event->topic_len) == 0) && (strncmp(event->data, "getTemp", event->data_len) == 0)){ 
             xEventGroupSetBits(tasksGroup, takeTemperature);
            break;
        } 
            //If topic is /tankesp32/light and message is toggleLight
         else if ((strncmp(event->topic, "/tankesp32/light ", event->topic_len) == 0) && (strncmp(event->data, "toggleLight", event->data_len) == 0)){ 
            xEventGroupSetBits(tasksGroup, toggleLight);
            break;
        }

         else {
            break;

        }

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



