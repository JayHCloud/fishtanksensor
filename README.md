I wanted to try ESPIDF and my fish tank needed a non-fluorescent light. Connects to MQTT and periodically takes the water temperature, sending it every 5 minutes.
If the temperature changes too quickly and it's higher than the average of the previously taken temperatures, the LED panel turns red as a warning. Very useful for water additions/changes.
The device also has an API-like task that listens to MQTT messages and toggles the lights/takes the temperature on command. This lets Home Assistant control it. 

Still a WIP but my fish like it 🫡 🐠
