// COMP2811 Coursework 2: data model

#include "model.hpp"


void QuakeModel::updateFromFile(const QString& filename)
{
  beginResetModel();
  dataset.loadData(filename.toStdString());
  endResetModel();
}


QVariant QuakeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole) {
    Quake q = dataset[index.row()];
    switch (index.column()) {
      case 0: return QVariant(q.getTime().c_str());
      case 1: return QVariant(q.getLatitude());
      case 2: return QVariant(q.getLongitude());
      case 3: return QVariant(q.getDepth());
      case 4: return QVariant(q.getMagnitude());
    }
  }

  return QVariant();
}


QVariant QuakeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (orientation == Qt::Vertical) {
    return QVariant(section + 1);
  }

  switch (section) {
    case 0: return QString("Time");
    case 1: return QString("Latitude");
    case 2: return QString("Longitude");
    case 3: return QString("Depth");
    case 4: return QString("Magnitude");
    default: return QVariant();
  }
}
