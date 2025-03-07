#pragma once

#include <QWidget>

class PixelLabel;
class QTreeWidget;
class QTreeWidgetItem;
class ScanScreen;

/// Topic tree on a party deck; the open entry's scan on a data window and its text on lined paper.
class EncyclopediaPage : public QWidget {
public:
    explicit EncyclopediaPage(QWidget* parent = nullptr);

private:
    void showEntry(QTreeWidgetItem* item);

    QTreeWidget* m_tree;
    ScanScreen* m_scan;
    PixelLabel* m_title;
    PixelLabel* m_body;
    QTreeWidgetItem* m_entry = nullptr;
};
