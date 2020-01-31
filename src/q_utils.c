/* The functions contained in this file are general
 * use utilities for writing C code to interact with
 * k objects within the C api
*/

// create q dictionary from list of items (s1;v1;s2;v2;...)
K xd0(I n, ...){
  va_list a;
  S s;
  K x, y= ktn(KS, n), z= ktn(0, n);
  y->n=0;z->n=0;
  va_start(a, n);
  for(; s= va_arg(a, S), s && (x= va_arg(a, K));)
    js(&y, ss(s)), jk(&z, x);
  va_end(a);
  return xD(y, z);
}
#define xd(...) xd0(0, __VA_ARGS__, (S) 0)

// check type
// letter as usual, + for table, ! for dict
static I checkType(const C* tc, ...){
  va_list args;
  K x;
  static C lt[256]= " tvunzdmpscfejihg xb*BX GHIJEFCSPMDZNUVT";
  static C b[256];
  const C* tc0= tc;
  I match=0;
  lt[20 + 98]= '+';
  lt[20 + 99]= '!';
  va_start(args, tc);
  for(; *tc;){
    match= 0;
    x= va_arg(args, K);
    if(!x){
      strcpy(b, "incomplete type string ");
      break;
    };
    if('[' == *tc){
      while(*tc && ']' != *tc){
        match= match || lt[20 + xt] == *tc;
        ++tc;
      }
    }
    else
      match= lt[20 + xt] == *tc;
    if(!match){
      strcat(strcpy(b, "type:expected "), tc0);
      break;
    };
    ++tc;
  }
  va_end(args);
  if(!match)
    krr(b);
  return match;
}

// print error if any and release K object.
// should return 0 to indicate mem free to kafka where needed in callback
static I printr0(K x){
  if(!x)
    return 0;
  if(KR == xt)
    fprintf(stderr, "%s\n", x->s);
  r0(x);
  return 0;
}

