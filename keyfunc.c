/*
 * Zentrale Tastatursteuerung
 * --------------------------
 * GTK ruft diese Funktion bei jedem Tastendruck auf. TRUE bedeutet: Arboretum
 * hat die Taste vollständig verarbeitet. FALSE gibt sie an das fokussierte
 * Eingabefeld weiter, damit dort beispielsweise Buchstaben erscheinen.
 */
static gboolean eingabefeld_verarbeitet_pfeiltaste(guint keyval,
                                                    GdkModifierType state)
{
  GtkWidget *fokus = gtk_window_get_focus(GTK_WINDOW(window));
  if (!fokus || !GTK_IS_EDITABLE(fokus))
    return FALSE;

  /* Kombinationen wie Strg+Pfeil und Shift+Pfeil gehören vollständig zur
   * üblichen Textbearbeitung des Eingabefelds. */
  if (state & gtk_accelerator_get_default_mod_mask())
    return TRUE;

  GtkEditable *editable = GTK_EDITABLE(fokus);
  int auswahl_start, auswahl_ende;
  if (gtk_editable_get_selection_bounds(editable, &auswahl_start, &auswahl_ende))
    return TRUE;

  int position = gtk_editable_get_position(editable);
  glong textlaenge = g_utf8_strlen(gtk_editable_get_text(editable), -1);

  return (keyval == GDK_KEY_Left && position > 0) ||
         (keyval == GDK_KEY_Right && position < textlaenge);
}

static gboolean keyfunc(GtkEventControllerKey *controller, guint keyval,
                        guint keycode, GdkModifierType state, gpointer data)
{
  GtkWidget *widget =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
  gboolean steuerung =
      (state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK;

  switch (keyval)
  {
    /* Navigation im Baum */
    case GDK_KEY_Down:
      runter(widget, data);
      break;
    case GDK_KEY_Right:
      if (eingabefeld_verarbeitet_pfeiltaste(keyval, state))
        return FALSE;
      rechts(widget, data);
      break;
    case GDK_KEY_Up:
      hoch(widget, data);
      break;
    case GDK_KEY_Left:
      if (eingabefeld_verarbeitet_pfeiltaste(keyval, state))
        return FALSE;
      links(widget, data);
      break;
    case GDK_KEY_Home:
      pos1(widget, data);
      break;
    case GDK_KEY_Tab:
      knotenwskwechseln();
      break;

    /* Bearbeiten und Dateioperationen */
    case GDK_KEY_Delete:
      if (steuerung)
        reset(data);
      else
        loeschen(data);
      break;
    case GDK_KEY_Insert:
      oeffnen(NULL, data);
      break;
    case GDK_KEY_F1:
      hilfe(NULL, NULL);
      break;

    /* Reine Umschalttasten verändern den Baum nicht. */
    case GDK_KEY_Shift_L:
    case GDK_KEY_Shift_R:
    case GDK_KEY_Control_L:
    case GDK_KEY_Control_R:
    case GDK_KEY_Caps_Lock:
    case GDK_KEY_ISO_Level3_Shift:
      break;

    /* Tastenkürzel mit Strg. Ohne Strg geht das Zeichen ans Eingabefeld. */
    case GDK_KEY_s:
    case GDK_KEY_S:
      if (steuerung)
      {
        speichernvor(NULL, NULL);
        break;
      }
      tempspeichern();
      return FALSE;

    case GDK_KEY_a:
    case GDK_KEY_A:
      if (steuerung)
      {
        exportdialog(NULL, data);
        break;
      }
      tempspeichern();
      return FALSE;

    case GDK_KEY_e:
    case GDK_KEY_E:
      if (steuerung)
      {
        ergebnisspalteanzeigen(data);
        break;
      }
      tempspeichern();
      return FALSE;

    case GDK_KEY_w:
    case GDK_KEY_W:
      if (steuerung)
      {
        wskergebnisspalteanzeigen(data);
        break;
      }
      tempspeichern();
      return FALSE;

    case GDK_KEY_minus:
      if (steuerung)
      {
        ueberstreichen();
        break;
      }
      tempspeichern();
      return FALSE;

    case GDK_KEY_u:
    case GDK_KEY_U:
      if (steuerung)
      {
        umwandeln(NULL, data);
        break;
      }
      tempspeichern();
      return FALSE;

    case GDK_KEY_z:
    case GDK_KEY_Z:
      if (steuerung)
      {
        rueckgaengig(widget, data);
        break;
      }
      tempspeichern();
      return FALSE;

    default:
      /* Normale Texteingabe wird gespeichert und dann von GtkEntry verarbeitet. */
      tempspeichern();
      return FALSE;
  }

  return TRUE;
}
