package diana.project.master_degree.api;
import diana.project.master_degree.config.Configurations;
import diana.project.master_degree.model.Sensors;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;
import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;
import java.util.List;

@Controller
@RequestMapping("/")
public class api {
    @Autowired
    private Configurations util;

    private JedisPool jedisPool = null;

    @RequestMapping(value = "/retrieveFirstsSensorValues", method = RequestMethod.POST)
    public @ResponseBody List<String> findFirstsValues(@RequestBody Sensors sensor, @RequestParam("rightLimitInterval") int rightLimitInterval) {
        List<String> retrieveMap = null;
        jedisPool = util.getJedisPool();
        try (Jedis jedis = jedisPool.getResource()) {
            String key = getListKey(sensor.getscope_of_sensor(), sensor.getSensorPlacement());


            //retrieves values from the 0 index to the index requested in the post request
            retrieveMap = jedis.lrange(key, 0, rightLimitInterval);
        }
        return retrieveMap;

    }

    @RequestMapping(value="/getAllValues", method = RequestMethod.GET)
            public List<String> getGetAll(@RequestBody Sensors sensor){
            List<String> getAll=null;
            jedisPool = util.getJedisPool();
        try (Jedis jedis = jedisPool.getResource()) {

        String key = getListKey(sensor.getscope_of_sensor(), sensor.getSensorPlacement());


            getAll = (List<String>) jedis.hgetAll(key);
    }
                return getAll;
            }

    private String getListKey(String scope_of_sensor,String sensorPlacement){
        return scope_of_sensor+":"+sensorPlacement;
    }


}
