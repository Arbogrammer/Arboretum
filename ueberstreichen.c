void ueberstreichen()
{
    GtkEntry *entry = GTK_ENTRY(gtk_window_get_focus(GTK_WINDOW(window)));
  
    GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(entry);
    gint position = gtk_editable_get_position(GTK_EDITABLE(entry));

    gtk_entry_buffer_insert_text(entry_buffer, position, "̅", -1);

}
