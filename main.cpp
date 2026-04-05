#include <QCoreApplication>
#include <QCommandLineParser>
#include <QStringList>
#include <QByteArray>
#include <QFile>
#include <QDebug>

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {

    QCoreApplication app(argc, argv);

    // Set application metadata
    QCoreApplication::setApplicationName("BerryBboot qCompress Utility");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription("Creates distro.zsmime if distro.smime exists");
    parser.addPositionalArgument("file", "The file to open.", "[file]");

    // A boolean option with multiple names (-f, --force)
     QCommandLineOption forceOption(QStringList() << "f" << "force",
        QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(forceOption);

    // CRITICAL: Must call process() to parse arguments
    parser.process(app);

    if (parser.isSet("help")) {
        return 0; // Exit after help is printed
    }

    // Retrieve values
    QStringList pArgs = parser.positionalArguments();
    if (pArgs.size() == 0) {
        // Handle case where optional argument was not provided
        pArgs.append("distro.smime");
    }
    else if (pArgs.size() > 1) {
      qDebug() << "Too many arguments supplied.";
      return 1;
    }

    // Original data
    QString name = pArgs.at(0);
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: file could not be opened.";
        return 1;
    }

    QByteArray originalData = file.readAll();
    file.close();

    // Now 'originalData' contains the full content of the file as a QByteArray

    // Compress the data
    QByteArray compressedData = qCompress(originalData, -1); // Use level 9 for maximum compression

    // Set output filename
    QString outName = name + ".z";

    // Special case for .smime files
    int lastDot = name.lastIndexOf('.');
    if (lastDot != -1) {
        QString extension = name.mid(lastDot + 1);
        QString filename = name.mid(0, lastDot);
        if (extension == "smime") {
          outName = filename + ".zsmime";
        }
    }
    QFile outFile(outName);

    // Check if output exists
    if (QFile::exists(outName)) {
        bool force = parser.isSet(forceOption);
        if (!force) {
          std::cout << "Overwrite " << outName.toStdString().c_str() << " (y/n)? " << std::flush;
          std::string confirm;
          std::getline(std::cin, confirm); // Reads full line including spaces
          if (confirm != "y") {
            qDebug() << "Operation aborted.";
            return 1;
          }
        }
    }

    // Save compressed data to a file
    if (outFile.open(QIODevice::WriteOnly)) {
        outFile.write(compressedData);
        outFile.close();
        qDebug() << "Compressed data written to" << outName;
    }

    // Load and decompress
    QFile inFile(outName);
    if (inFile.open(QIODevice::ReadOnly)) {
        QByteArray readData = inFile.readAll();
        inFile.close();

        QByteArray decompressedData = qUncompress(readData);
        if (!decompressedData.isEmpty()) {
            if (decompressedData == originalData) {
              qDebug() << "Decompressed data matches original data.";
            } else {
              qDebug() << "Decompressed data did not match:" << decompressedData;
            }
        } else {
            qDebug() << "Decompression failed!";
        }
    }

    return 0;
}
