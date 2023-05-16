package gg.com;

import org.eclipse.paho.client.mqttv3.MqttClient;

public class WioPubDisconnector extends Thread {
    private MqttController mqttController;
    private MqttClient mqttClient;

    WioPubDisconnector(MqttController mqttController, MqttClient mqttClient) {
        this.mqttController = mqttController;
        this.mqttClient = mqttClient;
    }

    @Override
    public void run() {
        try {
            mqttController.publish("stoppub;");
            mqttClient.disconnect();
            mqttClient.close();
        } catch (Exception e){
            e.printStackTrace();
        }
    }
}