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
    QFile file(pArgs.at(0));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: file could not be opened.";
        return 1;
    }

    QByteArray originalData = file.readAll();
    file.close();

    // Now 'originalData' contains the full content of the file as a QByteArray

    // Compress the data
    QByteArray compressedData = qCompress(originalData, -1); // Use level 9 for maximum compression

    // Save compressed data to a file
    QFile outFile("distro.zsmime");

    // Check if output exists
    if (QFile::exists("distro.zsmime")) {
        bool force = parser.isSet(forceOption);
        if (!force) {
          std::cout << "Overwrite distro.zsmime (y/n)? " << std::flush;
          std::string confirm;
          std::getline(std::cin, confirm); // Reads full line including spaces
          if (confirm != "y") {
            qDebug() << "Operation aborted.";
            return 1;
          }
        }
    }

    if (outFile.open(QIODevice::WriteOnly)) {
        outFile.write(compressedData);
        outFile.close();
        qDebug() << "Compressed data written to file.";
    }

    // Load and decompress
    QFile inFile("distro.zsmime");
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
