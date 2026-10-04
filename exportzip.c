/* Stored ZIP archives shared by ODT and DOCX. No external tools required.
 * Callers supply UTF-8 XML and ASCII entry names; no extra fields are written. */
static void export_zip_uint(GByteArray *out, guint32 value, int count) {
  for (int i = 0; i < count; i++) {
    guint8 byte = value & 255;
    g_byte_array_append(out, &byte, 1);
    value >>= 8;
  }
}

static GByteArray *export_zip(const char **names, const char **data, guint count) {
  GByteArray *out = g_byte_array_new();
  GByteArray *central = g_byte_array_new();
  for (guint i = 0; i < count; i++) {
    guint32 crc = 0xffffffff, len = strlen(data[i]), offset = out->len;
    guint16 n = strlen(names[i]);
    for (guint32 j = 0; j < len; j++) {
      crc ^= (guint8)data[i][j];
      for (int bit = 0; bit < 8; bit++)
        crc = (crc >> 1) ^ (0xedb88320U & (0U - (crc & 1)));
    }
    crc ^= 0xffffffff;
    export_zip_uint(out, 0x04034b50, 4);
    export_zip_uint(out, 20, 2); export_zip_uint(out, 0, 2); export_zip_uint(out, 0, 2);
    export_zip_uint(out, 0, 2); export_zip_uint(out, 33, 2); /* 1980-01-01 */
    export_zip_uint(out, crc, 4); export_zip_uint(out, len, 4); export_zip_uint(out, len, 4);
    export_zip_uint(out, n, 2); export_zip_uint(out, 0, 2);
    g_byte_array_append(out, (const guint8 *)names[i], n);
    g_byte_array_append(out, (const guint8 *)data[i], len);
    export_zip_uint(central, 0x02014b50, 4);
    export_zip_uint(central, 20, 2); export_zip_uint(central, 20, 2);
    export_zip_uint(central, 0, 2); export_zip_uint(central, 0, 2);
    export_zip_uint(central, 0, 2); export_zip_uint(central, 33, 2);
    export_zip_uint(central, crc, 4); export_zip_uint(central, len, 4); export_zip_uint(central, len, 4);
    export_zip_uint(central, n, 2); export_zip_uint(central, 0, 2); export_zip_uint(central, 0, 2);
    export_zip_uint(central, 0, 2); export_zip_uint(central, 0, 2); export_zip_uint(central, 0, 4);
    export_zip_uint(central, offset, 4);
    g_byte_array_append(central, (const guint8 *)names[i], n);
  }
  guint32 offset = out->len;
  g_byte_array_append(out, central->data, central->len);
  export_zip_uint(out, 0x06054b50, 4); export_zip_uint(out, 0, 2); export_zip_uint(out, 0, 2);
  export_zip_uint(out, count, 2); export_zip_uint(out, count, 2);
  export_zip_uint(out, central->len, 4); export_zip_uint(out, offset, 4); export_zip_uint(out, 0, 2);
  g_byte_array_unref(central);
  return out;
}

