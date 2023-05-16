package gg.com;

import org.eclipse.paho.client.mqttv3.*;

public class MqttController {
    private static final String PUB_TOPIC = "commands"; // topic to pub to
    private static final String SUB_TOPIC = "sensordata"; // topic to subscribe to
    private static final String URL = "tcp://127.0.0.1:1883";
    private static final String CLIENT_ID = "fromintellij";   // the app client ID name
    private static final int QOS = 0;
    MqttClient mqttClient;
     MqttController(){

         try {

             this.mqttClient = new MqttClient(URL, CLIENT_ID);

             mqttClient.connect();

         } catch (Exception e){}
     }
     MqttClient getClient () {
         return mqttClient;
     }
    void publish (String message){

        try {

            MqttMessage msg = new MqttMessage(message.getBytes());

            msg.setQos(QOS);

            msg.setRetained(true);

            mqttClient.publish(PUB_TOPIC, msg);

        } catch (Exception e){

            e.printStackTrace();
        }
    }
    public void subscribe(){

        try {

            mqttClient.setCallback(new MqttCallback() {
                public void connectionLost(Throwable cause) {}

                public void messageArrived(String topic, MqttMessage message) {}

                public void deliveryComplete(IMqttDeliveryToken token) {}
        });

        mqttClient.subscribe(SUB_TOPIC, QOS);

        } catch (Exception e) {

            e.printStackTrace();

        }
    }
}