/* --- Producer api ---
 * Function which is used to publish data from a client
 * to kafka main thread and ensure publishing is completed
*/

// producer api
EXP K4(kfkPub){
  rd_kafka_topic_t *rkt;
  if(!checkType("ii[CG][CG]", x, y, z, r))
    return KNL;
  if(!(rkt= topicIndex(x)))
    return KNL;
  if(rd_kafka_produce(rkt, y->i, RD_KAFKA_MSG_F_COPY, kG(z), z->n, kG(r), r->n, NULL))
    return krr((S) rd_kafka_err2str(rd_kafka_last_error()));
  return KNL;
}

// Wait until all outstanding producer requests are completed to ensure
// that the all messages have been processed prior to destroying a producer
EXP K2(kfkFlush){
  rd_kafka_t *rk;
  I qy=0;
  if(!checkType("i[hij]",x,y))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  SW(y->t){
    CS(-KH,qy=y->h);
    CS(-KI,qy=y->i);
    CS(-KJ,qy=y->j);
  }
  rd_kafka_resp_err_t err= rd_kafka_flush(rk,qy);
  if(KFK_OK != err)
    return krr((S) rd_kafka_err2str(err));
  return KNL;
}
