#include "XournalWidget.h"

#include <cmath>


#include <gdk/gdk.h>
#include "gui/Shadow.h"
#include "gui/inputdevices/InputContext.h"
#include "gui/Renderer.h"

XournalWidget::XournalWidget(std::shared_ptr<InputContext> inputContext, std::shared_ptr<Renderer> render): input(std::move(inputContext)), renderer(std::move(render)) {
    this->init();
    this->viewport = Rectangle<int>();
}

auto XournalWidget::init() -> void {
    this->drawingArea = gtk_drawing_area_new();
    gtk_widget_set_hexpand(this->drawingArea, true);
    gtk_widget_set_vexpand(this->drawingArea, true);
    g_signal_connect(G_OBJECT(drawingArea), "size-allocate", G_CALLBACK(XournalWidget::sizeAllocateCallback), this);
    g_signal_connect(G_OBJECT(drawingArea), "realize", G_CALLBACK(XournalWidget::realizeCallback), this);
    g_signal_connect(G_OBJECT(drawingArea), "draw", G_CALLBACK(XournalWidget::drawCallback), this);
    if (this->input) {
        this->input->connect(this);
    }
}

XournalWidget::~XournalWidget() {
    gtk_widget_destroy(this->drawingArea);
};

auto XournalWidget::getGtkWidget() -> GtkWidget* { return this->drawingArea; }


auto XournalWidget::repaintArea(const Rectangle<int>& rect) -> void {
    if(rect.intersects(viewport.get()))
}

auto XournalWidget::getVisibleArea() -> Rectangle<int> {
    return this->viewport;
}

auto inline XournalWidget::queueRedraw() -> void {
    gtk_widget_queue_draw(this->drawingArea);
}

auto XournalWidget::sizeAllocateCallback(GtkWidget* drawingArea, GdkRectangle* allocation, XournalWidget* self)
        -> void {
    if (gtk_widget_get_realized(drawingArea)) {
        gdk_window_move_resize(gtk_widget_get_window(self->drawingArea), allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }
    Rectangle size = self->renderer->getDocumentSize();
    int width = allocation->width + SIZE_EXTENSION;
    int height = allocation->height + SIZE_EXTENSION;
    if (width >= size.width)
        width = size.width;
    if (height >= size.height)
        height = size.height;
    gtk_widget_set_size_request(drawingArea, width, height);
}

auto XournalWidget::realizeCallback(GtkWidget* drawingArea, XournalWidget* self) -> void {
    // Disable event compression
    gdk_window_set_event_compression(gtk_widget_get_window(drawingArea), false);
}

auto XournalWidget::drawCallback(GtkWidget* drawArea, cairo_t* cr, XournalWidget* self) -> gboolean {
    double x1 = NAN, x2 = NAN, y1 = NAN, y2 = NAN;
    cairo_clip_extents(cr, &x1, &y1, &x2, &y2);

    Rectangle clippingRect(self->x - 10, self->y - 10, x2 - x1 + 20, y2 - y1 + 20);

    self->renderer->render(cr, clippingRect, self->scale);

    return true;
}
