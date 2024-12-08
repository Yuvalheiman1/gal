// COMP2811 Coursework 2: data model

#pragma once

#include <QAbstractTableModel>
#include "dataset.hpp"

class QuakeModel: public QAbstractTableModel
{
  public:
    QuakeModel(QObject* parent = nullptr): QAbstractTableModel(parent) {}
    void updateFromFile(const QString&);
    bool hasData() const { return dataset.size() > 0; }

    int rowCount(const QModelIndex& index) const { return dataset.size(); }
    int columnCount(const QModelIndex& index) const { return 5; }
    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    double meanDepth() const { return dataset.meanDepth(); }
    double meanMagnitude() const { return dataset.meanMagnitude(); }

  private:
    QuakeDataset dataset;
};
