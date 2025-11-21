#ifndef MONGOSERVICE_H
#define MONGOSERVICE_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types.hpp>

class MongoService {
public:
    MongoService();
    QString obtenerMarkdown(const QString& dificultad);
    QString leerArchivoMarkdown(const QString& ruta);

private:
    mongocxx::instance instance;
    mongocxx::client client;
};

#endif // MONGOSERVICE_H
