/* --- Topic api ---
 * The functions in this file handle the creation, removal
 * and interaction with kafka topics
*/

// Retrieve the index associated with a kafka topic
rd_kafka_topic_t *topicIndex(K x){
  if((((UI) xi < topics->n)&&kS(topics)[xi]))
    return(rd_kafka_topic_t *)kS(topics)[xi];
  else
    return(rd_kafka_topic_t *)(S)krr("unknown topic");
}

// Set a topic configuration based on a q dict
static K loadTopConf(rd_kafka_topic_conf_t *conf, K x){
  char b[512];
  J i;
  for(i= 0; i < xx->n; ++i){
    if(RD_KAFKA_CONF_OK !=rd_kafka_topic_conf_set(conf, kS(xx)[i], kS(xy)[i], b, sizeof(b)))
      return krr((S) b);
  }
  return knk(0);
}

// Create a named and configured Kafka topic associated with a client
EXP K3(kfkTopic){
  rd_kafka_topic_t *rkt;
  rd_kafka_t *rk;
  rd_kafka_topic_conf_t *rd_topic_conf;
  if(!checkType("is!",x ,y ,z))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  rd_topic_conf= rd_kafka_topic_conf_new();
  loadTopConf(rd_topic_conf, z);
  rkt= rd_kafka_topic_new(rk, y->s, rd_topic_conf);
  js(&topics, (S) rkt); 
  return ki(topics->n - 1);
}

// Delete a numerically assigned kafka topic
EXP K1(kfkTopicDel){
  rd_kafka_topic_t *rkt;
  if(!checkType("i", x))
    return KNL;
  if(!(rkt= topicIndex(x)))
    return KNL;
  rd_kafka_topic_destroy(rkt);
  kS(topics)[x->i]= (S) 0;
  return KNL;
}

// Return the name of a numerically assigned kafka topic
EXP K1(kfkTopicName){
  rd_kafka_topic_t *rkt;
  if(!checkType("i", x))
    return KNL;
  if(!(rkt= topicIndex(x)))
    return KNL;
  return ks((S) rd_kafka_topic_name(rkt));
}

