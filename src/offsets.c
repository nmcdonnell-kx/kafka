// https://github.com/edenhill/librdkafka/wiki/Manually-setting-the-consumer-start-offset
EXP K3(kfkAssignOffsets){
  rd_kafka_t *rk;
  rd_kafka_topic_partition_list_t *t_partition;
  rd_kafka_resp_err_t err;
  if(!checkType("i[sS]!", x,y,z))
    return KNL;
  if(!checkType("IJ",kK(z)[0],kK(z)[1]))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  t_partition = rd_kafka_topic_partition_list_new(z->n);
  plistoffsetdict(y,z,t_partition);
  if(KFK_OK != (err=rd_kafka_assign(rk,t_partition)))
    return krr((S) rd_kafka_err2str(err));
  rd_kafka_topic_partition_list_destroy(t_partition);
  return knk(0);
}

EXP K4(kfkCommitOffsets){
  rd_kafka_resp_err_t err;
  rd_kafka_t *rk;rd_kafka_topic_partition_list_t *t_partition;
  if(!checkType("i[sS]!b", x, y, z, r))
    return KNL;
  if(!checkType("IJ",kK(z)[0],kK(z)[1]))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  t_partition = rd_kafka_topic_partition_list_new(z->n);
  plistoffsetdict(y,z,t_partition);
  if(KFK_OK != (err= rd_kafka_commit(rk, t_partition,r->g)))
    return krr((S) rd_kafka_err2str(err));
  rd_kafka_topic_partition_list_destroy(t_partition);
  return knk(0);
}

EXP K3(kfkCommittedOffsets){
  K r;
  rd_kafka_resp_err_t err;
  rd_kafka_t *rk;rd_kafka_topic_partition_list_t *t_partition;
  if(!checkType("i[sS]!", x, y, z))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  if(!checkType("IJ",kK(z)[0],kK(z)[1]))
    return KNL;
  t_partition = rd_kafka_topic_partition_list_new(z->n);
  plistoffsetdict(y,z,t_partition);
  if(KFK_OK != (err= rd_kafka_committed(rk, t_partition,5000)))
    return krr((S) rd_kafka_err2str(err));
  r=decodeParList(t_partition);
  rd_kafka_topic_partition_list_destroy(t_partition);
  return r;
}

EXP K3(kfkPositionOffsets){
  K r;
  rd_kafka_resp_err_t err;
  rd_kafka_t *rk;rd_kafka_topic_partition_list_t *t_partition;
  if(!checkType("i[sS]!", x, y, z))
    return KNL;
  if(!checkType("IJ",kK(z)[0],kK(z)[1]))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  t_partition = rd_kafka_topic_partition_list_new(z->n);
  plistoffsetdict(y,z,t_partition);
  if(KFK_OK != (err= rd_kafka_position(rk, t_partition)))
    return krr((S) rd_kafka_err2str(err));
  r=decodeParList(t_partition);
  rd_kafka_topic_partition_list_destroy(t_partition);
  return r;
}
