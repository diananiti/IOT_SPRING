package diana.project.master_degree.model;

public class Sensors {
    private String sensorPlacement;

    private String DHT_11_Humid;

    private String DHT_11_Temp;

    private String Hear_rate;

    private String scope_of_sensor;


    public Sensors(){

    }

    public String getscope_of_sensor() {
        return scope_of_sensor;
    }

    public void setscope_of_sensor(String scope_of_sensor) {
        this.scope_of_sensor = scope_of_sensor;
    }


    public String getSensorPlacement() {
        return sensorPlacement;
    }

    public void setSensorPlacement(String sensorPlacement) {
        this.sensorPlacement = sensorPlacement;
    }


    public String getHear_rate(){ return Hear_rate;}
    public void setHear_rate(String Hear_rate){this.Hear_rate=Hear_rate;}

    public String getDHT_11_Humid() {
        return DHT_11_Humid;
    }

    public void setDHT_11_Humid(String DHT_11_Humid) {
        this.DHT_11_Humid = DHT_11_Humid;
    }




    public String getDHT_11_Temp() {
        return DHT_11_Temp;
    }

    public void setDHT_11_Temp(String DHT_11_Temp) {
        this.DHT_11_Temp = DHT_11_Temp;
    }

}
