static gboolean keyfunc (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  switch(event->keyval)
  {
    case GDK_KEY_Down: runter(widget, data); break;
    case GDK_KEY_Right: rechts(widget, data); break;
    case GDK_KEY_Up: hoch(widget, data); break;
    case GDK_KEY_Left: links(widget, data); break;
    case GDK_KEY_Home: pos1(widget, data); break;
    case GDK_KEY_Delete:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        reset(data); break;
      }
      else
      {
        loeschen(data); break;
      }
    case GDK_KEY_Insert: oeffnen(NULL,data); break;
    case GDK_KEY_F1: hilfe(NULL,NULL); break;
    case GDK_KEY_Tab: knotenwskwechseln(); break;
    case GDK_KEY_Shift_L: break;
    case GDK_KEY_Shift_R: break;
    case GDK_KEY_Control_L: break;
    case GDK_KEY_Control_R: break;
    case GDK_KEY_Caps_Lock: break;
    case GDK_KEY_ISO_Level3_Shift: break;
    case GDK_KEY_s: case GDK_KEY_S:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        speichernvor(NULL,NULL); break;
      }
      else
      {
        tempspeichern(); return FALSE; break;
      }
    case GDK_KEY_a: case GDK_KEY_A:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        if(labelein)
        {
          exportdialog(data); break;
        }
        else
        {
           return FALSE; break;
        }
      }
      else
      {
        tempspeichern(); return FALSE; break;
      }
    case GDK_KEY_e: case GDK_KEY_E:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        ergebnisspalteanzeigen(data); break;
      }
      else
      {
        tempspeichern(); return FALSE; break;
      }
    case GDK_KEY_w: case GDK_KEY_W:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        wskergebnisspalteanzeigen(data); break;
      }
      else
      {
        tempspeichern(); return FALSE; break;
      }
    case GDK_KEY_minus:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        ueberstreichen(); break;
      }
      else
      {
        tempspeichern(); return FALSE; break;
      }
    case GDK_KEY_u: case GDK_KEY_U:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        umwandeln(NULL,data); break;
      }
      else
      {
        tempspeichern(); return FALSE; break;
      }
    case GDK_KEY_z: case GDK_KEY_Z:
      if ((event->state & gtk_accelerator_get_default_mod_mask()) == GDK_CONTROL_MASK)
      {
        rueckgaengig(widget, data); break;
      }
      else
      {
        tempspeichern(); return FALSE; break;
      }
    default: tempspeichern(); return FALSE; break;
  }
  return TRUE;
}
