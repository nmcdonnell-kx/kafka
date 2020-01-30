/* --- Producer api ---
 * Function which is used to publish data from a client
 * to kafka main thread.
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
