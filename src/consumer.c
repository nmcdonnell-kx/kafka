/* --- Consumer api ---
 * The functions in this file allow a user to subscribe to topics for all possible
 * partitions via the sub functionality or to specific topic partitions assignments 
 * or via the assign functionality
*/ 

// Used in the assignment of topic/partitions pairs from which to assign consumption
static V ptlist(K dict, rd_kafka_topic_partition_list_t *t_partition);
// Used in the deletion of assignments from topic/partitions list
static V ptlistdel(K dict, rd_kafka_topic_partition_list_t *t_partition);
// Used for subscription calls, same as above but with ability to define offset
static V plistoffsetdict(K topic,K partitions,rd_kafka_topic_partition_list_t *t_partition);

/* The following functions define how subscriptions can be defined
 * in Kafka a subscription will consume from a balancd consumer group for 
 * a particular topic (control over the partitions is limited to availability)
*/

// Subscribe to a topic from a specified client
EXP K3(kfkSub){
  rd_kafka_resp_err_t err;
  rd_kafka_t *rk;rd_kafka_topic_partition_list_t *t_partition;
  J i;
  I *p;
  if(!checkType("is[I!]", x, y, z))
    return KNL;
  if(!(rk = clientIndex(x)))
    return KNL;
  if(KFK_OK != (err = rd_kafka_subscription(rk, &t_partition)))
    return krr((S)rd_kafka_err2str(err));
  if(z -> t == XD){
    if(!checkType("IJ", kK(z)[0], kK(z)[1]))
      return KNL;
    plistoffsetdict(y,z,t_partition);
  }
  else
    for(p = kI(z), i = 0; i < z->n; ++i)
      rd_kafka_topic_partition_list_add(t_partition, y->s, p[i]);
  if(KFK_OK != (err= rd_kafka_subscribe(rk, t_partition)))
    return krr((S) rd_kafka_err2str(err));
  rd_kafka_topic_partition_list_destroy(t_partition);
  return knk(0);
}

// Return the current subscriptions to a client from a consumer
EXP K1(kfkSubscription){
  K r;
  rd_kafka_topic_partition_list_t *t;
  rd_kafka_t *rk;
  rd_kafka_resp_err_t err;
  if (!checkType("i", x))
    return KNL;
  if (!(rk = clientIndex(x)))
    return KNL;
  if (KFK_OK != (err= rd_kafka_subscription(rk, &t)))
    return krr((S)rd_kafka_err2str(err));
  r = decodeParList(t);
  rd_kafka_topic_partition_list_destroy(t);
  return r;
}

// Unsubscribe from all topics associated with a client
EXP K1(kfkUnsub){
  rd_kafka_t *rk;
  rd_kafka_resp_err_t err;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  err= rd_kafka_unsubscribe(rk);
  if(KFK_OK != err)
    return krr((S) rd_kafka_err2str(err));
  return knk(0);
}


/* The following set of functions define how consumption is to be **assigned**
 * this forces consumption to only occur from a given set of topics with 
 * associated partitions and provides more control over the process of consumption
 * than the subscribe functionality
*/

// Assign the partitions for specified topics to consume from a specified client
EXP K2(kfkAssign){
  rd_kafka_t *rk;
  rd_kafka_topic_partition_list_t *t_partition;
  rd_kafka_resp_err_t err;
  if(!checkType("i!", x, y))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  t_partition = rd_kafka_topic_partition_list_new(y->n);
  // topic-partition assignment
  ptlist(y,t_partition);
  if(KFK_OK != (err=rd_kafka_assign(rk,t_partition)))
    return krr((S) rd_kafka_err2str(err));
  rd_kafka_topic_partition_list_destroy(t_partition);
  return KNL;
}

EXP K2(kfkAssignAdd){
  rd_kafka_t *rk;
  rd_kafka_topic_partition_list_t *t;
  rd_kafka_resp_err_t err;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  // retrieve the current assignment
  if(KFK_OK != (err=rd_kafka_assignment(rk, &t)))
    return krr((S)rd_kafka_err2str(err));
  ptlist(y,t);
  if(KFK_OK != (err=rd_kafka_assign(rk,t)))
    return krr((S) rd_kafka_err2str(err));
  rd_kafka_topic_partition_list_destroy(t);
  return 0;
}

EXP K2(kfkAssignDel){
  rd_kafka_t *rk;
  rd_kafka_topic_partition_list_t *t;
  rd_kafka_resp_err_t err;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  // retrieve the current assignment
  if(KFK_OK != (err=rd_kafka_assignment(rk, &t)))
    return krr((S)rd_kafka_err2str(err));
  ptlistdel(y,t);
  if(KFK_OK != (err=rd_kafka_assign(rk,t)))
    return krr((S) rd_kafka_err2str(err));
  rd_kafka_topic_partition_list_destroy(t);
  return 0;
}

// Return the current consumption assignment for a specified client
EXP K1(kfkAssignment){
  K r;
  rd_kafka_topic_partition_list_t *t;
  rd_kafka_t *rk;
  rd_kafka_resp_err_t err;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  if(KFK_OK != (err=rd_kafka_assignment(rk, &t)))
    return krr((S)rd_kafka_err2str(err));
  r = decodeParList(t);
  rd_kafka_topic_partition_list_destroy(t);
  return r;
}

// partition-topic assignment for consumption (dict -> topic!associated partitions)
static V ptlist(K dict, rd_kafka_topic_partition_list_t *t_partition){
  K dk=kK(dict)[0],dv=knk(0);
  J i,j;
  for(i = 0; i < dk->n; i++){
    dv = kK(kK(dict)[1])[i];
    for(j = 0; j < dv->n; j++)
      rd_kafka_topic_partition_list_add(t_partition,kS(dk)[i],kJ(dv)[j]);
  }
}

static V ptlistdel(K dict,rd_kafka_topic_partition_list_t *t_partition){
  K dk=kK(dict)[0],dv=knk(0);
  J i,j;
  for(i = 0; i < dk->n; i++){
    dv = kK(kK(dict)[1])[i];
    for(j = 0; j < dv->n; j++)
      rd_kafka_topic_partition_list_del(t_partition,kS(dk)[i],kJ(dv)[j]);
  }
}

// partition-topic-offset assignment for subscription
static V plistoffsetdict(K topic,K partitions,rd_kafka_topic_partition_list_t *t_partition){
  K dk=kK(partitions)[0],dv=kK(partitions)[1];
  I*p;J*o,i;
  p=kI(dk);o=kJ(dv);
  for(i= 0; i < dk->n; ++i){
    rd_kafka_topic_partition_list_add(t_partition, topic -> s, p[i]);
    rd_kafka_topic_partition_list_set_offset(t_partition, topic -> s, p[i],o[i]);
  }
}

/* The following set of functions provide the mechanism
 * by which messages can be consumed from a defined client
 * these control the flow of messages that can be retrieved
 * from the kafka main thread message buffer
*/

// Poll the client for available messages to consume
J pollClient(rd_kafka_t *rk, J timeout, J UNUSED(maxcnt)) {
  K r;
  J n = 0;
  rd_kafka_message_t *msg;
  rd_kafka_type_t rk_type;
  rk_type= rd_kafka_type(rk);
  if(rk_type == RD_KAFKA_PRODUCER) {
    n= rd_kafka_poll(rk, timeout);
    return n;
  }
  while((msg= rd_kafka_consumer_poll(rk, timeout))) {
    r= decodeMsg(rk,msg);
    printr0(k(0, ".kfk.consumecb", r, KNL));
    rd_kafka_message_destroy(msg);
    n++;
  }
  return n;
}

// Manual poll the feed to consume available messages
EXP K3(kfkPoll){
  J n = 0;
  rd_kafka_t *rk;
  if(!checkType("ijj", x, y, z))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  n=pollClient(rk,y->j,z->j);
  return kj(n);
}

// Defined callback function to allow q main thread to 
// consume available messages from kafka main thread
EXP K kfkCallback(I d){
  char buf[1024];J i,n,consumed=0;
  /*MSG_DONTWAIT - set in sd1(-h,...) */
  while(0 < (n = recv(d, buf, sizeof(buf), 0)))
    consumed+=n;
  // pass consumed to poll for possible batching
  for(i = 0; i < clients->n; i++)
    pollClient((rd_kafka_t*)kS(clients)[i], 0, consumed);
  return KNL;
}



/* The functions below allow the consumption from particular
 * topic partition pairs to be started and stopped
 * NB. within the current api design consumption stop works correctly
 * but restart does not appear to be possible
*/

EXP K3(kfkConsumeStart){
  rd_kafka_topic_t *rkt;
  if(!checkType("iij", x, y, z))
    return KNL;
  if(!(rkt = topicIndex(x)))
    return KNL;
  if(!(0 == rd_kafka_consume_start(rkt,y->i,z->j)))
    return krr((S)rd_kafka_err2str(rd_kafka_last_error()));
  return KNL;
}

EXP K2(kfkConsumeStop){
  rd_kafka_topic_t *rkt;
  if(!checkType("ii", x, y))
    return KNL;
  if(!(rkt = topicIndex(x)))
    return KNL;
  if(!(0 == rd_kafka_consume_stop(rkt,y->i)))
    return KNL;
  return KNL;
}

