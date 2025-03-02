#pragma once

#include <QWidget>

class BioScan;
class QLabel;
class QTreeWidget;
class QTreeWidgetItem;

/// Collapsible tree of topics on the left, the selected entry (scan image and text) on the right.
class EncyclopediaPage : public QWidget {
public:
    explicit EncyclopediaPage(QWidget* parent = nullptr);

private:
    void showEntry(QTreeWidgetItem* item);

    QTreeWidget* m_tree;
    BioScan* m_scan;
    QLabel* m_title;
    QLabel* m_body;
    QTreeWidgetItem* m_entry = nullptr;
};
