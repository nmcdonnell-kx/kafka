/* --- Decode functions ---
 * The functions contained in this file are used to convert
 * topic/partition lists, metadata and messaged from kakfa
*/

// rd_kafka_metadata_broker: `id`host`port!(id;host;port)
K decodeMetaBroker(rd_kafka_metadata_broker_t *x){
  return xd("id", ki(x->id), "host", ks(x->host), "port", ki(x->port));
}

K decodeMetaPart(rd_kafka_metadata_partition_t *p){
  K x= ktn(KI, p->replica_cnt);
  J i;
  for(i= 0; i < xn; ++i)
    kI(x)[i]= p->replicas[i];
  K y= ktn(KI, p->isr_cnt);
  for(i= 0; i < y->n; ++i)
    kI(y)[i]= p->isrs[i];
  return xd("id", ki(p->id), "err", ks((S) rd_kafka_err2str(p->err)),
            "leader",ki(p->leader), "replicas", x, "isrs", y);
}

// rd_kafka_metadata_topic:`topic`partitions`err!(string;partition list;err)
K decodeMetaTopic(rd_kafka_metadata_topic_t *t){
  K x= ktn(0, 0);
  J i;
  for(i= 0; i < t->partition_cnt; ++i)
    jk(&x, decodeMetaPart(&t->partitions[i]));
  return xd("topic", ks(t->topic),
            "err",ks((S) rd_kafka_err2str(t->err)),"partitions", x);
}

// rd_kafka_metadata: 
// `brokers`topics`orig_broker_id`orig_broker_name!(brokerlist;topic list;int;string)
K decodeMeta(const rd_kafka_metadata_t *meta) {
  K x= ktn(0, 0);
  J i;
  for(i= 0; i < meta->broker_cnt; ++i)
    jk(&x, decodeMetaBroker(&meta->brokers[i]));
  K y= ktn(0, 0);
  for(i= 0; i < meta->topic_cnt; ++i)
    jk(&y, decodeMetaTopic(&meta->topics[i]));
  return xd("orig_broker_id", ki(meta->orig_broker_id),
            "orig_broker_name",ks(meta->orig_broker_name),
            "brokers", x, "topics", y);
}

K decodeTopPar(rd_kafka_topic_partition_t *tp){
  return xd("topic", ks((S) tp->topic), "partition", ki(tp->partition),
            "offset", kj(tp->offset),
            "metadata",kpn(tp->metadata, tp->metadata_size));
}

K decodeParList(rd_kafka_topic_partition_list_t *t){
  K r;J i;
  if(!t)return knk(0);
  r= ktn(0, t->cnt);
  for(i= 0; i < r->n; ++i)
    kK(r)[i]= decodeTopPar(&t->elems[i]);
  return r;
}

static J pu(J u){return 1000000LL*(u-10957LL*86400000LL);}

// `mtype`topic`partition`data`key`offset`opaque
K decodeMsg(const rd_kafka_t* rk, const rd_kafka_message_t *msg) {
  K x= ktn(KG, msg->len), y=ktn(KG, msg->key_len), z;
  J ts= rd_kafka_message_timestamp(msg, NULL);
  memmove(kG(x), msg->payload, msg->len);
  memmove(kG(y), msg->key, msg->key_len);
  z= ktj(-KP, ts > 0 ? pu(ts) : nj);
  return xd0(8,
    "mtype", msg->err ? ks((S) rd_kafka_err2name(msg->err)) : r1(S0),
    "topic", msg->rkt ? ks((S) rd_kafka_topic_name(msg->rkt)) : r1(S0),
    "client", ki(indexClient(rk)), "partition", ki(msg->partition), "offset", kj(msg->offset),
    "msgtime", z, "data", x, "key", y, (S) 0);
}
