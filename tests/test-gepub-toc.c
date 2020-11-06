#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <libgepub/gepub.h>

gchar *buf = NULL;
gchar *buf2 = NULL;
gchar *tmpbuf;

GtkTextBuffer *page_buffer;
GtkWidget *PAGE_LABEL;

#define PTEST1(...) printf (__VA_ARGS__)
#define PTEST2(...) buf = g_strdup_printf (__VA_ARGS__);\
                    tmpbuf = buf2;\
                    buf2 = g_strdup_printf ("%s%s", buf2, buf);\
                    g_free (buf);\
                    g_free (tmpbuf)
#define PTEST PTEST1

#define TEST(f,arg...) PTEST ("\n### TESTING " #f " ###\n\n"); f (arg); PTEST ("\n\n");

static void
reload_current_chapter (GepubWidget *widget)
{
    gchar *txt = g_strdup_printf ("%02.2f", gepub_widget_get_pos (widget));
    gtk_label_set_text (GTK_LABEL (PAGE_LABEL), txt);
    g_free (txt);
}

// static void
// update_text (GepubDoc *doc)
// {
//     GList *l, *chunks;
//     GtkTextIter start, end;

//     gtk_text_buffer_get_start_iter (page_buffer, &start);
//     gtk_text_buffer_get_end_iter (page_buffer, &end);
//     gtk_text_buffer_delete (page_buffer, &start, &end);

//     chunks = gepub_doc_get_text (doc);

//     for (l=chunks; l; l = l->next) {
//         GepubTextChunk *chunk = GEPUB_TEXT_CHUNK (l->data);
//         if (chunk->type == GEPUBTextHeader) {
//             gtk_text_buffer_insert_at_cursor (page_buffer, "\n", -1);
//             gtk_text_buffer_get_end_iter (page_buffer, &end);
//             gtk_text_buffer_insert_with_tags_by_name (page_buffer, &end, chunk->text, -1, "head",  NULL);
//             gtk_text_buffer_insert_at_cursor (page_buffer, "\n", -1);
//         } else if (chunk->type == GEPUBTextNormal) {
//             gtk_text_buffer_insert_at_cursor (page_buffer, "\n", -1);
//             gtk_text_buffer_insert_at_cursor (page_buffer, chunk->text, -1);
//             gtk_text_buffer_insert_at_cursor (page_buffer, "\n", -1);
//         } else if (chunk->type == GEPUBTextItalic) {
//             gtk_text_buffer_get_end_iter (page_buffer, &end);
//             gtk_text_buffer_insert_with_tags_by_name (page_buffer, &end, chunk->text, -1, "italic",  NULL);
//         } else if (chunk->type == GEPUBTextBold) {
//             gtk_text_buffer_get_end_iter (page_buffer, &end);
//             gtk_text_buffer_insert_with_tags_by_name (page_buffer, &end, chunk->text, -1, "bold",  NULL);
//         }
//     }
// }

#if 0
static void
print_replaced_text (GepubDoc *doc)
{
    GBytes *content;
    gsize s;
    const guchar *data;
    content = gepub_doc_get_current_with_epub_uris (doc);

    data = g_bytes_get_data (content, &s);
    printf ("\n\nREPLACED:\n%s\n", data);
    g_bytes_unref (content);
}
#endif

static void
button_pressed (GtkToolButton *button, GepubWidget *widget)
{
    GepubDoc *doc = gepub_widget_get_doc (widget);
    printf("CLICKED %s\n",  gtk_tool_button_get_label (button));

    if (!strcmp (gtk_tool_button_get_label (button), "< chapter")) {
        gepub_doc_go_prev (doc);
    } else if (!strcmp (gtk_tool_button_get_label (button), "chapter >")) {
        gepub_doc_go_next (doc);
    } else     if (!strcmp (gtk_tool_button_get_label (button), "< vchapter")) {
        gepub_doc_go_prev_virtual (doc);
    } else if (!strcmp (gtk_tool_button_get_label (button), "vchapter >")) {
        gepub_doc_go_next_virtual (doc);
    } else if (!strcmp (gtk_tool_button_get_label (button), "paginate")) {
        gboolean b = gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON (button));
        gepub_widget_set_paginate (widget, b);
    } else if (!strcmp (gtk_tool_button_get_label (button), "< page")) {
        gepub_widget_page_prev (widget);
    } else if (!strcmp (gtk_tool_button_get_label (button), "page >")) {
        gepub_widget_page_next (widget);
    } else if (!strcmp (gtk_tool_button_get_label (button), "margin +")) {
        gepub_widget_set_margin (widget, gepub_widget_get_margin (widget) + 20);
    } else if (!strcmp (gtk_tool_button_get_label (button), "margin -")) {
        gepub_widget_set_margin (widget, gepub_widget_get_margin (widget) - 20);
    } else if (!strcmp (gtk_tool_button_get_label (button), "font +")) {
        gint f = gepub_widget_get_fontsize (widget);
        f = f ? f : 12;
        printf ("font %d\n", f);
        gepub_widget_set_fontsize (widget, f + 2);
    } else if (!strcmp (gtk_tool_button_get_label (button), "font -")) {
        gint f = gepub_widget_get_fontsize (widget);
        f = f ? f : 12;
        printf ("font %d\n", f);
        gepub_widget_set_fontsize (widget, f - 2);
    } else if (!strcmp (gtk_tool_button_get_label (button), "line height +")) {
        gfloat l = gepub_widget_get_lineheight (widget);
        l = l ? l : 1.5;
        gepub_widget_set_lineheight (widget, l + 0.1);
    } else if (!strcmp (gtk_tool_button_get_label (button), "line height -")) {
        gfloat l = gepub_widget_get_lineheight (widget);
        l = l ? l : 1.5;
        gepub_widget_set_lineheight (widget, l - 0.1);
    }
    // update_text (doc);
    //print_replaced_text (doc);
}

static void
test_open (const char *path)
{
    GepubArchive *a;
    GList *list_files;
    gint i;
    gint size;

    a = gepub_archive_new (path);
    list_files = gepub_archive_list_files (a);
    if (!list_files) {
        PTEST ("ERROR: BAD epub file");
        g_object_unref (a);
        return;
    }

    size = g_list_length (list_files);
    PTEST ("%d\n", size);
    for (i = 0; i < size; i++) {
        PTEST ("file: %s\n", (char *)g_list_nth_data (list_files, i));
        g_free (g_list_nth_data (list_files, i));
    }

    g_list_free (list_files);

    g_object_unref (a);
}

static void
find_xhtml (gchar *key, GepubResource *value, gpointer data)
{
    gchar **d = (gchar **)data;
    if (g_strcmp0 (value->mime, "application/xhtml+xml") == 0) {
        *d = value->uri;
    }
}

static void
test_read (const char *path)
{
    GepubArchive *a;
    GList *list_files = NULL;
    const guchar *buffer;
    gchar *file = NULL;
    gsize bufsize;
    GBytes *bytes;
    GepubDoc *doc;
    GHashTable *ht;

    a = gepub_archive_new (path);

    doc = gepub_doc_new (path, NULL);
    ht = (GHashTable*)gepub_doc_get_resources (doc);
    g_hash_table_foreach (ht, (GHFunc)find_xhtml, &file);

    bytes = gepub_archive_read_entry (a, file);
    if (bytes) {
        buffer = g_bytes_get_data (bytes, &bufsize);
        PTEST ("doc:%s\n----\n%s\n-----\n", file, buffer);
        g_bytes_unref (bytes);
    }

    g_list_foreach (list_files, (GFunc)g_free, NULL);
    g_list_free (list_files);

    g_object_unref (a);
}

static void
test_root_file (const char *path)
{
    GepubArchive *a;
    gchar *root_file = NULL;

    a = gepub_archive_new (path);

    root_file = gepub_archive_get_root_file (a);
    PTEST ("root file: %s\n", root_file);
    if (root_file)
        g_free (root_file);

    g_object_unref (a);
}

static void
test_doc_name (const char *path)
{
    GepubDoc *doc = gepub_doc_new (path, NULL);
    gchar *title = gepub_doc_get_metadata (doc, GEPUB_META_TITLE);
    gchar *lang = gepub_doc_get_metadata (doc, GEPUB_META_LANG);
    gchar *id = gepub_doc_get_metadata (doc, GEPUB_META_ID);
    gchar *author = gepub_doc_get_metadata (doc, GEPUB_META_AUTHOR);
    gchar *description = gepub_doc_get_metadata (doc, GEPUB_META_DESC);
    gchar *cover = gepub_doc_get_cover (doc);
    gchar *cover_mime = gepub_doc_get_resource_mime_by_id (doc, cover);

    PTEST ("title: %s\n", title);
    PTEST ("author: %s\n", author);
    PTEST ("id: %s\n", id);
    PTEST ("lang: %s\n", lang);
    PTEST ("desc: %s\n", description);
    PTEST ("cover: %s\n", cover);
    PTEST ("cover mime: %s\n", cover_mime);

    g_free (title);
    g_free (lang);
    g_free (id);
    g_free (author);
    g_free (description);
    g_free (cover);
    g_object_unref (G_OBJECT (doc));
}

static void
pk (gchar *key, GepubResource *value, gpointer data)
{
    PTEST ("%s: %s, %s\n", key, value->mime, value->uri);
}

static void
test_doc_resources (const char *path)
{
    GepubDoc *doc;
    GHashTable *ht;
    GBytes *ncx;
    const guchar *data;
    gsize size;

    doc = gepub_doc_new (path, NULL);
    ht = (GHashTable*)gepub_doc_get_resources (doc);
    g_hash_table_foreach (ht, (GHFunc)pk, NULL);

    ncx = gepub_doc_get_resource_by_id (doc, "ncx");
    data = g_bytes_get_data (ncx, &size);
    PTEST ("ncx:\n%s\n", data);
    g_bytes_unref (ncx);

    g_object_unref (G_OBJECT (doc));
}

static void
test_doc_spine (const char *path)
{
    GepubDoc *doc = gepub_doc_new (path, NULL);
    int id = 0;

    do {
        PTEST ("%d: %s\n", id++, gepub_doc_get_current_id (doc));
    } while (gepub_doc_go_next (doc));

    g_object_unref (G_OBJECT (doc));
}

static void
print_doc_toc_recursive(GepubDoc *doc, GList *nav)
{
    while (nav && nav->data) {
        GepubNavPoint *point = (GepubNavPoint*)nav->data;
        PTEST ("%02d %03ld: %-40s %s # %s\n", point->chapter, point->playorder, point->label, point->content, point->params);
        if (point->children)
        {
            print_doc_toc_recursive(doc, point->children);
        }
        nav = nav->next;
    }
}

static void
test_doc_toc (const char *path)
{
    GepubDoc *doc = gepub_doc_new (path, NULL);

    GList *nav = gepub_doc_get_toc (doc);
    print_doc_toc_recursive(doc, nav);

    g_object_unref (G_OBJECT (doc));
}

static void
destroy_cb (GtkWidget *window,
            GtkWidget *view)
{
    g_signal_handlers_disconnect_by_func (G_OBJECT (view),
                                          reload_current_chapter,
                                          view);
    gtk_main_quit ();
}

// typedef struct {
//     GtkWidget *w_dlg_file_choose;       // Pointer to file chooser dialog box
//     GtkWidget *w_img_main;              // Pointer to image widget
// } TestGepubTocWidgets;

enum {
	EV_DOCUMENT_LINKS_COLUMN_MARKUP,
	EV_DOCUMENT_LINKS_COLUMN_LINK,
	EV_DOCUMENT_LINKS_COLUMN_EXPAND,
	EV_DOCUMENT_LINKS_COLUMN_PAGE_LABEL,
	EV_DOCUMENT_LINKS_COLUMN_NUM_COLUMNS
};

typedef struct {
	GtkTreeModel *model;
	GtkTreeIter  *parent;
} TocModelData;

static void
epub_document_make_tree_entry(GepubNavPoint* navpoint, TocModelData* user_data)
{
	GtkTreeIter tree_iter;
	// EvLink *link = NULL;
	// gboolean expand;
	char *title_markup;

	// if (navpoint->children) {
	// 	expand=TRUE;
	// }
	// else {
	// 	expand=FALSE;
	// }

	// EvLinkDest *ev_dest = NULL;
	// EvLinkAction *ev_action;

	/* We shall use a EV_LINK_DEST_TYPE_PAGE for page links,
	 * and a EV_LINK_DEST_TYPE_HLINK(custom) for refs on a page of type url#label
	 * because we need both dest and page label for this.
	 */

	// if (g_strrstr(navpoint->content, "#") == NULL) {
	// 	ev_dest = ev_link_dest_new_page(navpoint->playorder);
	// }
	// else {
	// 	ev_dest = ev_link_dest_new_hlink((gchar*)navpoint->content, navpoint->playorder);
	// }

	// ev_action = ev_link_action_new_dest (ev_dest);

	// link = ev_link_new((gchar*)navpoint->label, ev_action);

	gtk_tree_store_append (GTK_TREE_STORE (user_data->model), &tree_iter, user_data->parent);
	title_markup = g_strdup(navpoint->label);//g_markup_escape_text (navpoint->label, -1);

	gtk_tree_store_set (GTK_TREE_STORE (user_data->model), &tree_iter,
			    0, title_markup,
                1, navpoint->content,
                2, navpoint->params,
			    -1);

	if (navpoint->children) {
		TocModelData cbstruct;
		cbstruct.parent = &tree_iter;
		cbstruct.model = user_data->model;
		g_list_foreach (navpoint->children,(GFunc)epub_document_make_tree_entry,&cbstruct);
	}

	g_free (title_markup);
}

static void
on_selection_changed(GtkWidget *selection, GepubWidget *widget)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gboolean success = gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter);

    if (!success) {
        return;
    }

    GepubDoc *doc = gepub_widget_get_doc (widget);

    gchar *navpoint_content;
    gchar *navpoint_params;
    gchar *title;
    gtk_tree_model_get(model, &iter, 0, &title, -1);
    gtk_tree_model_get(model, &iter, 1, &navpoint_content, -1);
    gtk_tree_model_get(model, &iter, 2, &navpoint_params, -1);

    gint chapter = gepub_doc_resource_uri_to_chapter (doc, navpoint_content);

    printf("row activated: %s >>> %s # %s (chapter = %d)\n", title, navpoint_content, navpoint_params, chapter);

    if (chapter >= 0) {
        gepub_widget_set_chapter_href(widget, chapter, navpoint_params);
        // gepub_widget_set_chapter_virtual(widget, );
        // gepub_widget_chapter_href(widget, navpoint_params);
    }
}


int
main (int argc, char **argv)
{

    GtkBuilder *builder;
    GtkWidget  *window_main;
    // TestGepubTocWidgets *widgets = g_slice_new(TestGepubTocWidgets);

    GtkWidget *scrolled;

    GtkWidget *b_chapter_next;
    GtkWidget *b_chapter_prev;
    GtkWidget *b_vchapter_next;
    GtkWidget *b_vchapter_prev;
    GtkWidget *b_page_next;
    GtkWidget *b_page_prev;

    GtkWidget *paginate;

    // GtkTextBuffer *buffer;

    GepubDoc *doc;

    GtkWidget *gepub_widget;
    GtkTreeStore *toc_treestore;

    if (argc < 2) {
        printf ("you should provide an .epub file\n");
        return 1;
    }

    gtk_init (&argc, &argv);

    builder = gtk_builder_new_from_file("tests/test-with-toc.glade");
    window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

    gepub_widget = gepub_widget_new ();
    webkit_settings_set_enable_developer_extras (webkit_web_view_get_settings (WEBKIT_WEB_VIEW (gepub_widget)), TRUE);

    g_signal_connect (window_main, "destroy", G_CALLBACK(destroy_cb), gepub_widget);

    doc = gepub_doc_new (argv[1], NULL);
    if (!doc) {
        perror ("BAD epub FILE");
        return -1;
    }

    gepub_widget_set_doc (GEPUB_WIDGET (gepub_widget), doc);


    /* ----------------------------- TOC -------------------------------------------------------------------------------------------------- */
    toc_treestore = GTK_TREE_STORE(gtk_builder_get_object(builder, "epub_toc_treestore"));

    GtkTreeIter iter;
	gtk_tree_store_append (toc_treestore, &iter, NULL);
    gtk_tree_store_set (toc_treestore, &iter, 0, gepub_doc_get_metadata(doc, GEPUB_META_TITLE), -1);
    gtk_tree_store_set (toc_treestore, &iter, 1, gepub_doc_get_current_path(doc), -1);
    gtk_tree_store_set (toc_treestore, &iter, 2, "", -1);

    TocModelData linkStruct;
	linkStruct.model = GTK_TREE_MODEL(toc_treestore);
	linkStruct.parent = &iter;

    GList *toc = gepub_doc_get_toc(doc);
	if (toc) {
		g_list_foreach (toc, (GFunc)epub_document_make_tree_entry, &linkStruct);
	}

    GtkTreeSelection *toc_view = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "epub_toc_selected_row"));
    g_signal_connect (toc_view, "changed", (GCallback)on_selection_changed, GEPUB_WIDGET (gepub_widget));
    /* ----------------------------- TOC -------------------------------------------------------------------------------------------------- */

    scrolled = GTK_WIDGET(gtk_builder_get_object(builder, "epub_view"));
    gtk_container_add (GTK_CONTAINER (scrolled), gepub_widget);

    b_chapter_prev = GTK_WIDGET(gtk_builder_get_object(builder, "button_chapter_prev"));
    g_signal_connect (b_chapter_prev, "clicked", (GCallback)button_pressed, GEPUB_WIDGET (gepub_widget));
    b_chapter_next = GTK_WIDGET(gtk_builder_get_object(builder, "button_chapter_next"));
    g_signal_connect (b_chapter_next, "clicked", (GCallback)button_pressed, GEPUB_WIDGET (gepub_widget));
    b_vchapter_prev = GTK_WIDGET(gtk_builder_get_object(builder, "button_vchapter_prev"));
    g_signal_connect (b_vchapter_prev, "clicked", (GCallback)button_pressed, GEPUB_WIDGET (gepub_widget));
    b_vchapter_next = GTK_WIDGET(gtk_builder_get_object(builder, "button_vchapter_next"));
    g_signal_connect (b_vchapter_next, "clicked", (GCallback)button_pressed, GEPUB_WIDGET (gepub_widget));
    b_page_prev = GTK_WIDGET(gtk_builder_get_object(builder, "button_page_prev"));
    g_signal_connect (b_page_prev, "clicked", (GCallback)button_pressed, GEPUB_WIDGET (gepub_widget));
    b_page_next = GTK_WIDGET(gtk_builder_get_object(builder, "button_page_next"));
    g_signal_connect (b_page_next, "clicked", (GCallback)button_pressed, GEPUB_WIDGET (gepub_widget));

    paginate = GTK_WIDGET(gtk_builder_get_object(builder, "paginated"));
    g_signal_connect (paginate, "clicked", (GCallback)button_pressed, GEPUB_WIDGET (gepub_widget));

    gtk_widget_show_all (window_main);

    // Testing all
    // TEST(test_open, argv[1])
    // TEST(test_read, argv[1])
    TEST(test_root_file, argv[1])
    TEST(test_doc_name, argv[1])
    // TEST(test_doc_resources, argv[1])
    TEST(test_doc_spine, argv[1])
    // TEST(test_doc_toc, argv[1])

    gtk_main ();

    g_object_unref (doc);

    return 0;
}
