int ggt(long long int x, long long int y) {
  long long int c;

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
