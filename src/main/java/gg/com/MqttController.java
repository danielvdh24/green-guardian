package gg.com;

import org.eclipse.paho.client.mqttv3.*;
import java.time.format.DateTimeFormatter;
import java.time.LocalDateTime;

public class MqttController implements Runnable {
    private static final String PUB_TOPIC = "commands"; // topic to pub to
    private static final String SUB_TOPIC = "sensordata"; // topic to subscribe to
    private static final String URL = "tcp://127.0.0.1:1883";
    private static final String CLIENT_ID = "fromintellij";   // the app client ID name
    private static final int QOS = 0;
    private static String[] dataBuffer = new String[16];
    MqttClient mqttClient;

    MqttController() throws Exception {
        for(int i=0;i<16;i++)dataBuffer[i]=null;
        try {
            this.mqttClient = new MqttClient(URL, CLIENT_ID);
            mqttClient.connect();
        } catch (Exception e){
            throw new Exception("Bad Connection");
        }
    }
    MqttClient getClient() {
         return mqttClient;
    }

    void publish(String message) throws Exception {

        try {
            MqttMessage msg = new MqttMessage(message.getBytes());
            msg.setQos(QOS);
            msg.setRetained(true);
            mqttClient.publish(PUB_TOPIC, msg);
        } catch (Exception e){
            throw new Exception("Bad Connection");
        }
    }
    public void subscribe(App app) throws Exception {
            mqttClient.setCallback(new MqttCallback() {
                public void connectionLost(Throwable cause) {
                    app.reconnectBroker();
                }
                public void messageArrived(String topic, MqttMessage message) {
                    String text = message.toString();
                    dataBuffer[text.charAt(0)-'a']=text.substring(1);
                    boolean messagecomplete = true;
                    for(int i=0;i<16&&messagecomplete;i++)if(dataBuffer[i]==null)messagecomplete = false;
                    if(messagecomplete)
                    {
                        String result = "";
                        for(int i=0;i<16;i++)
                        {
                            result += dataBuffer[i];
                            dataBuffer[i] = null;
                        }
                        DocWriter.write("GraphData.txt",result);
                    }
                }
                public void deliveryComplete(IMqttDeliveryToken token) {}
            });
            mqttClient.subscribe(SUB_TOPIC, QOS);
        }
    @Override
    public void run() {

        while (mqttClient.isConnected()) {

                try {

                    DateTimeFormatter dtf = DateTimeFormatter.ofPattern("HHmm");

                    LocalDateTime now = LocalDateTime.now();

                    MqttMessage msg = new MqttMessage(dtf.format(now).getBytes());

                    msg.setQos(QOS);

                    msg.setRetained(false);

                    mqttClient.publish(PUB_TOPIC, msg);

                    Thread.sleep(60000);

                } catch (Exception e) {

                    e.printStackTrace();
                }
            }
        }
    }