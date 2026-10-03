long long int ggt(long long int x, long long int y) {
  long long int c;

  /* The positive magnitude of LLONG_MIN is not representable as signed
   * long long.  Leaving such a fraction unreduced is safer than overflowing
   * while trying to negate it. */
  if (x == LLONG_MIN || y == LLONG_MIN)
    return 0;
  if (x < 0)
    x = -x;
  if (y < 0)
    y = -y;
  while (y != 0) {
    c = x % y;
    x = y;
    y = c;
  }
  return x;
}
