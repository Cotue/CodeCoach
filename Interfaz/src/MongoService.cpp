#include "MongoService.h"
#include <bsoncxx/json.hpp>
#include <QDebug>
#include <QTextStream>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <QFile>

MongoService::MongoService()
    : client(mongocxx::uri("mongodb+srv://Josue:Gollini2699@leetcode.7ukanre.mongodb.net/?retryWrites=true&w=majority"))
{
}

QString MongoService::leerArchivoMarkdown(const QString& ruta)
{
    QFile file(ruta);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "No se pudo abrir el archivo markdown.";

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    return in.readAll();
}

QString MongoService::obtenerMarkdown(const QString& dificultad)
{
    auto db = client["leetcodeDb"];
    auto collection = db["problems"];

    // Builder actualizado
    bsoncxx::builder::basic::document filter_builder{};
    filter_builder.append(
        bsoncxx::builder::basic::kvp("difficulty", dificultad.toStdString())
    );

    auto result = collection.find_one(filter_builder.view());
    if (!result) {
        return "No existe un documento con esa dificultad.";
    }

    bsoncxx::document::view view = result->view();

    if (!view["markdown_path"]) {
        return "El documento no contiene 'markdown_path'.";
    }

    // Acceso correcto en v4.0.0
    auto val = view["markdown_path"].get_string().value;
    QString ruta = QString::fromStdString(std::string(val.data(), val.length()));

    return leerArchivoMarkdown(ruta);
}


