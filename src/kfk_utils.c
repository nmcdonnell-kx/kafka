/* --- Kafka Utilities ---
 * The following functions are general utilities which do not
 * explicitly fit in other sections
*/

// Returns the number of threads currently being used by librdkafka
EXP K kfkThreadCount(K UNUSED(x)){return ki(rd_kafka_thread_cnt());}

// Returns the numeric representation of the rdkafka version
EXP K kfkVersion(K UNUSED(x)){return ki(rd_kafka_version());}

// Returns the human readable librdkafka version
EXP K kfkVersionSym(K UNUSED(x)){return ks((S)rd_kafka_version_str());}

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

// Current length of producer queue, these are the messages
// waiting to be sent to, or acknowledged by, a broker.
EXP K1(kfkOutQLen){
  rd_kafka_t *rk;
  if(!(rk= clientIndex(x)))
    return KNL;
  return ki(rd_kafka_outq_len(rk));
}

// logger level is set based on Severity levels in syslog
//  https://en.wikipedia.org/wiki/Syslog#Severity_level
EXP K2(kfkSetLoggerLevel){
  rd_kafka_t *rk;
  I qy=0;
  if(!checkType("i[hij]",x,y))
    return KNL;
  if(!(rk=clientIndex(x)))
    return KNL;
  SW(y->t){
    CS(-KH,qy=y->h);
    CS(-KI,qy=y->i);
    CS(-KJ,qy=y->j);
  }
  rd_kafka_set_log_level(rk, qy);
  return KNL;
}

// Returns the full list of kafka error codes with the associated
// descriptions
EXP K kfkExportErr(K UNUSED(dummy)){
  const struct rd_kafka_err_desc *errdescs;
  size_t i,n;
  K x= ktn(0, 0), y= ktn(0, 0), z= ktn(0, 0);
  rd_kafka_get_err_descs(&errdescs, &n);
  for(i= 0; i < n; ++i)
    if(errdescs[i].code) {
      jk(&x, ki(errdescs[i].code));
      jk(&y, ks((S)(errdescs[i].name ? errdescs[i].name : "")));
      jk(&z, kp((S)(errdescs[i].desc ? errdescs[i].desc : "")));
    }
  return xT(xd("errid", x, "code", y, "desc", z));
}

// Metadata associated with a specified Kafka client
EXP K1(kfkMetadata){
  const struct rd_kafka_metadata *meta;
  K r;
  rd_kafka_t *rk;
  if(!checkType("i", x))
    return KNL;
  if(!(rk= clientIndex(x)))
    return KNL;
  rd_kafka_resp_err_t err= rd_kafka_metadata(rk, 1, NULL, &meta, 5000);
  if(KFK_OK != err)
    return krr((S) rd_kafka_err2str(err));
  r= decodeMeta(meta);
  rd_kafka_metadata_destroy(meta);
  return r;
}
