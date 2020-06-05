/* --- Client api ---
 * The purpose of this file is to act as a centralised location for
 * functions related to the definition of new producer/consumer
 * clients within the Kafka kdb api
*/


// These functions provide the ability to retrieve the information
// associated with the client index defined in kdb+
rd_kafka_t *clientIndex(K x){
  if((((UI) xi < clients->n)&&kS(clients)[xi]))
    return (rd_kafka_t *)kS(clients)[xi];
  else
    return (rd_kafka_t *)(S)krr("unknown client");
}

I indexClient(const rd_kafka_t *rk){
  int i;
  for (i = 0; i < clients->n; ++i)
    if(rk==(rd_kafka_t *)kS(clients)[i]) return i;
  return ni;
}


// The following definitions define the callbacks which can be invoked by the api

// Should return 0 to indicate mem free to kafka
static I statscb(rd_kafka_t*UNUSED(rk), S json, size_t json_len, V*UNUSED(opaque)){
  return printr0(k(0, (S) ".kfk.statcb", kpn(json, json_len), KNL));
}

static V logcb(const rd_kafka_t *UNUSED(rk), int level, const char *fac,
               const char *buf) {
  printr0(k(0, (S) ".kfk.logcb", ki(level), kp((S) fac), kp((S) buf), KNL));
}

static V offsetcb(rd_kafka_t *rk, rd_kafka_resp_err_t err, 
                  rd_kafka_topic_partition_list_t*offsets, V*UNUSED(opaque)){
  printr0(k(0, (S) ".kfk.offsetcb", ki(indexClient(rk)), 
          kp((S)rd_kafka_err2str(err)), decodeParList(offsets),KNL));
}

static V drcb(rd_kafka_t*rk,const rd_kafka_message_t *msg,V*UNUSED(opaque)){
  printr0(k(0,(S)".kfk.drcb",ki(indexClient(rk)), decodeMsg(rk,msg),KNL));
}

static V errcb(rd_kafka_t *rk, int err, const char *reason, V*UNUSED(opaque)){
  printr0(k(0, (S) ".kfk.errcb", ki(indexClient(rk)), ki(err), kp((S) reason), KNL));
}

static V throttlecb(rd_kafka_t *rk, const char *brokername, int32_t brokerid,
                    int throttle_time_ms, V*UNUSED(opaque)){
  printr0(k(0,(S) ".kfk.throttlecb", ki(indexClient(rk)),
          kp((S)brokername), ki(brokerid), ki(throttle_time_ms),KNL));
}


// Set the configuration to be used by a consumer/producer for a client as defined in a q dict
static K loadConf(rd_kafka_conf_t *conf, K x){
  char b[512];
  J i;
  for(i= 0; i < xx->n; ++i){
    if(RD_KAFKA_CONF_OK !=rd_kafka_conf_set(conf, kS(xx)[i], kS(xy)[i], b, sizeof(b))){
      return krr((S) b);
    }
  }
  return knk(0);
}

// x:client type p - producer, c - consumer
// y:config dict sym->sym
EXP K2(kfkClient){
  rd_kafka_type_t type;
  rd_kafka_t *rk;
  rd_kafka_conf_t *conf;
  rd_kafka_queue_t *queue;
  rd_kafka_resp_err_t err;
  char b[512];
  if(!checkType("c!", x, y))
    return KNL;
  if('p' != xg && 'c' != xg)
    return krr("type: unknown client type");
  type= 'p' == xg ? RD_KAFKA_PRODUCER : RD_KAFKA_CONSUMER;
  if(!loadConf(conf= rd_kafka_conf_new(), y))
    return KNL;
  rd_kafka_conf_set_stats_cb(conf,statscb);
  rd_kafka_conf_set_log_cb(conf,logcb);
  rd_kafka_conf_set_dr_msg_cb(conf,drcb);
  rd_kafka_conf_set_offset_commit_cb(conf,offsetcb);
  rd_kafka_conf_set_throttle_cb(conf,throttlecb);
  rd_kafka_conf_set_error_cb(conf,errcb);
  if(RD_KAFKA_CONF_OK !=rd_kafka_conf_set(conf, "log.queue", "true", b, sizeof(b)))
    return krr((S) b);
  if(!(rk= rd_kafka_new(type, conf, b, sizeof(b))))
    return krr(b);
  /* Redirect logs to main queue */
  rd_kafka_set_log_queue(rk,NULL);
  /* Redirect rd_kafka_poll() to consumer_poll() */
  if(type == RD_KAFKA_CONSUMER){
    if(KFK_OK != (err = rd_kafka_poll_set_consumer(rk)))
      return krr((S)rd_kafka_err2str(err));
      queue = rd_kafka_queue_get_consumer(rk);
  }
  else
    queue = rd_kafka_queue_get_main(rk);
  if(!queue)
    return krr((S)"Error retrieving the queue");
  rd_kafka_queue_io_event_enable(queue,spair[1],"X",1);
  js(&clients, (S) rk);
  return ki(clients->n - 1);
}

// Destroy a Kafka handle associated with a defined client
EXP K1(kfkClientDel){
  rd_kafka_t *rk;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  rd_kafka_consumer_close(rk);
  rd_kafka_destroy(rk);
  kS(clients)[x->i]= (S) 0;
  return KNL;
}

// Return the kafka handle name for a defined client
EXP K1(kfkClientName){
  rd_kafka_t *rk;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  return ks((S) rd_kafka_name(rk));
}

// Return the broker assigned group member id for a specified client
EXP K1(kfkClientMemberId){
  rd_kafka_t *rk;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  return ks(rd_kafka_memberid(rk));
}

