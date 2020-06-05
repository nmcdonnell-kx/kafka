\d .kfk
LIBPATH:`:libkfk 2:
funcs:(
    // .kfk.init[]:i
  (`kfkInit;1);

  /* Client management functions clients.c */
    // .kfk.Client[client_type:c;conf:S!S]:i
  (`kfkClient;2);
    // .kfk.ClientDel[client_id:i]:_
  (`kfkClientDel;1);
    // .kfk.ClientName[client_id:i]:s
  (`kfkClientName;1);
    // .kfk.ClientMemberId[client_id:i]:s
  (`kfkClientMemberId;1);

  /* Topic management functions 'topics.c' */  
    // .kfk.Topic[client_id:i;topicname:s;conf:S!S]:i
  (`kfkTopic;3);
    // .kfk.TopicDel[topic_id:i]:_
  (`kfkTopicDel;1);
    // .kfk.TopicName[topic_id:i]:s
  (`kfkTopicName;1);

  /* Message publish management 'producer.c '*/
    // .kfk.Pub[topic_id:i;partid:i;data;key]:_
  (`kfkPub;4);
    // .kfk.Flush[producer_id:i;timeout_ms:i]:()
  (`kfkFlush;2);

  /* Subscribe/assign functionality 'consumer.c' */
    // .kfk.Sub[client_id:i;topicname:s;partition_list|partition_offsets:I!J]:()
  (`kfkSub;3);
    // .kfk.Unsub[client_id:i]:()
  (`kfkUnsub;1);
    // .kfk.Subscription[client_id:i]
  (`kfkSubscription;1);
    // .kfk.Poll[client_id:i;timeout;max_messages]
  (`kfkPoll;3);
    // .kfk.Assign[client_id:i;topics:[sS];partitions:I!J]:()
  (`kfkAssign;2);
    // .kfk.AssignAdd[client_id:i;topics:[sS];partitions:I!J]:()
  (`kfkAssignAdd;2);
    // .kfk.AssignDel[client_id:i;topics:[sS];partitions:I!J]:()
  (`kfkAssignDel;2);
    // .kfk.Assignment[client_id:i]:xD
  (`kfkAssignment;1);
    // .kfk.ConsumeStart[client_id:i;topic:[iI];partition:[jJ]]:()
  (`kfkConsumeStart;3);
    // .kfk.ConsumeStop[client_id:i;topic:i]:()
  (`kfkConsumeStop;2);

  /* Offset functionality */
    // .kfk.CommitOffsets[client_id;topic:s;partition_offsets:I!J;async:b]:()
  (`kfkCommitOffsets;4);
    // .kfk.PositionOffsets[client_id:i;topic:s;partition_offsets:I!J]:partition_offsets
  (`kfkPositionOffsets;3);
    // .kfk.CommittedOffsets[client_id:i;topic:s;partition_offsets:I!J]:partition_offsets
  (`kfkCommittedOffsets;3);
    // .kfk.AssignOffsets[client_id:i;topic:s;partition_offsets:I!J]:()
  (`kfkAssignOffsets;3);

  /* Utility functions */
    // .kfk.Metadata[client_id:i]:S!()
  (`kfkMetadata;1);
    // .kfk.Threadcount[]:i
  (`kfkThreadCount;1);
    // .kfk.VersionSym[]:s
  (`kfkVersionSym;1);
    // .kfk.SetLoggerLevel[client_id:i;int_level:i]:()
  (`kfkSetLoggerLevel;2);
    // .kfk.Version[]:i
  (`kfkVersion;1);
    // .kfk.ExportErr[]:T
  (`kfkExportErr;1);
    // .kfk.OutQLen[client_id:i]:i
  (`kfkOutQLen;1);
    // .kfk.fileDump[client_id:i]:()
  (`kfkstateDump;1);
    // .kfk.untiTest[]:()
  (`kfkunitTest;1)
  );

// binding functions from dictionary funcs using rule
// kfk<Name> -> .kfk.<Name>
.kfk,:(`$3_'string funcs[;0])!LIBPATH@/:funcs


// Initialize the system setting up socketpair and populating 
// C arrays for client & topic management
Init[];

// Current version of librdkafka
Version:Version[];

// Table with all errors return by kafka with codes and description
Errors:ExportErr[];


// projection function for handling int/long lists of partitions for offset functions
osetp:{[cf;x;y;z]cf[x;y;$[99h=type z;z;("i"$z,())!count[z]#0]]}
// Allow Offset functionality to take topics as a list in z argument
CommittedOffsets:osetp[CommittedOffsets;;]
PositionOffsets :osetp[PositionOffsets;;]


// Stop consumption from specified topics and associated partitions
ConsumerStop:{[x;y]
  // Current assignment definition
  assign:(,'/)Assignment x;
  dict:key[p]!assign[`partition]@value p:group assign`topic;
  dict}


// Unassigned partition.
// The unassigned partition is used by the producer API for messages
// that should be partitioned using the configured or default partitioner.
PARTITION_UA:-1i


// taken from librdkafka.h
OFFSET.BEGINNING: -2     /**< Start consuming from beginning of kafka partition queue: oldest msg */
OFFSET.END:       -1     /**< Start consuming from end of kafka partition queue: next msg */
OFFSET.STORED:    -1000  /**< Start consuming from offset retrieved from offset store */
OFFSET.INVALID:   -1001  /**< Invalid offset */


// Producer client code
PRODUCER:"p"
Producer:Client[PRODUCER;]

// Consumer client code
CONSUMER:"c"
Consumer:{
  if[not `group.id in key[y];'"Consumers must define a `group.id within their configuration"];
  .kfk.Client[x;y]}[CONSUMER]


// table with kafka statistics
stats:() 	


// CALLBACKS -  should not be deleted or renamed and be present in .kfk namespace
// https://docs.confluent.io/current/clients/librdkafka/rdkafka_8h.html

// statistics provided by kafka about current state (rd_kafka_conf_set_stats_cb)
statcb:{[j]
  s:.j.k j;
  if[all `ts`time in key s;
    s[`ts]:-10957D+`timestamp$s[`ts]*1000;
    s[`time]:-10957D+`timestamp$1000000000*s[`time]];
  .kfk.stats,::enlist s;
  delete from `.kfk.stats where i<count[.kfk.stats]-100;}

// logger callback(rd_kafka_conf_set_log_cb)
logcb:{[level;fac;buf] show -3!(level;fac;buf);}

// PRODUCER: delivery callback (rd_kafka_conf_set_dr_msg_cb )
drcb:{[cid;msg]}

errcb:{[idx;err_num;err_msg;knl]}

throttlecb:{[brokernm;brokerid;throttletm]}

// CONSUMER: offset commit callback(rd_kafka_conf_set_offset_commit_cb)
offsetcb:{[cid;err;offsets]}

// Main callback for consuming messages(including errors)
consumecb:{[msg]}

\d .