package diana.project.master_degree.config;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.data.redis.connection.jedis.JedisConnectionFactory;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.data.redis.serializer.GenericToStringSerializer;
import redis.clients.jedis.JedisPool;
import redis.clients.jedis.JedisPoolConfig;

@Configuration
@ComponentScan("diana.project")
public class Configurations {

        private static JedisPool jedisPool;

        @Value("${spring.redis.host}")
        String redisHost;

        @Value("${spring.redis.port}")
        int redisPort;

        @Value("${redis.timeout}")
        int redisTimeout;


        public void initializeJedisPool() {
            if (jedisPool == null) {
                jedisPool = new JedisPool(new JedisPoolConfig(),
                        this.redisHost,
                        this.redisPort,
                        this.redisTimeout);

            }
        }


        public JedisPool getJedisPool() {
            if (jedisPool == null) {
                initializeJedisPool();
            }
            return jedisPool;
        }


        public void destroyJedisPool() {
            if (jedisPool != null && !jedisPool.isClosed()) {
                jedisPool.close();
                jedisPool.destroy();
            }
        }

        @Bean
        JedisConnectionFactory jedisConnectionFactory() {
            return new JedisConnectionFactory();
        }

        @Bean
        public RedisTemplate<String, Object> redisTemplate() {
            final RedisTemplate<String, Object> template = new RedisTemplate<String, Object>();
            template.setConnectionFactory(jedisConnectionFactory());
            //
            template.setValueSerializer(new GenericToStringSerializer<Object>(Object.class));
          //
            return template;
        }
}
