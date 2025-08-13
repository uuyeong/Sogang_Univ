/*
 * absVal(x): return the absolute value of x.
 * - Assume that INT_MIN (-2^31) is not provided as an input.
 * - Ex) absVal(2) = 2, absVal(-1) = 1
 */
int absVal(int x) {
  int t = x>>31;
  x = (x+t)^t;
  return x; 
}
