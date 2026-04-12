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
    QCoreApplication::setApplicationName("BerryBoot qCompress Utility");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription("Compress and expand files compatible with Qt");
    parser.addPositionalArgument("file", "The file to open.", "[file]");

    // A boolean option with multiple names (-f, --force)
     QCommandLineOption forceOption(QStringList() << "f" << "force",
        QCoreApplication::translate("main", "Overwrite existing files."));
    parser.addOption(forceOption);

    // A boolean option with multiple names (-z, --zlib)
     QCommandLineOption zlibOption(QStringList() << "z" << "zlib",
        QCoreApplication::translate("main", "Enforce zlib compatibility."));
    parser.addOption(zlibOption);

    // A boolean option with multiple names (-x, --extract)
     QCommandLineOption exOption(QStringList() << "x" << "extract",
        QCoreApplication::translate("main", "Extract compressed data."));
    parser.addOption(exOption);

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
    bool extract = parser.isSet(exOption);
    bool force = parser.isSet(forceOption);
    bool zlib = parser.isSet(zlibOption);
    if (zlib) {
      qDebug() << "Warning: zlib mode";
      // Compression to zlib .zz format is supported.
      // - However official tools (pigz) are required to decompress these.
      // The Qt algorithm does not support decompressing zlib .zz files
      // by design due to requiring the length of the original data.
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
    QByteArray compressedData;
    if (extract) {
      if (zlib) {
        // not supported
      }
      compressedData = qUncompress(originalData);
      if (compressedData.isEmpty()) {
        qDebug() << "Operation aborted.";
        return 1;
      }
    }
    else {
      compressedData = qCompress(originalData, -1); // Use level 9 for maximum compression
    }

    // Set output filename
    QString outName;
    int lastDot = name.lastIndexOf('.');
    if (extract) {
      if (lastDot == -1) {
        qDebug() << "Don't know how to extract without extension yet.";
        return 1;
      }
      QString extension = name.mid(lastDot + 1);
      QString filename = name.mid(0, lastDot);

      if (extension == "zsmime") {
        outName = filename + ".smime";
      }
      else {
        outName = filename;
      }
    }
    else {
      if (zlib) {
        outName = name + ".zz";
      }
      else {
        outName = name + ".z";
        // Special case for .smime files
        if (lastDot != -1) {
            QString extension = name.mid(lastDot + 1);
            QString filename = name.mid(0, lastDot);
            if (extension == "smime") {
              outName = filename + ".zsmime";
            }
        }
      }
    }
    QFile outFile(outName);

    // Check if output exists
    if (QFile::exists(outName)) {
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

    QByteArray header;
    if (zlib) {
      // strip 4 byte header
      compressedData.remove(0, 4);
    }

    // Save compressed data to a file
    if (outFile.open(QIODevice::WriteOnly)) {
        outFile.write(compressedData);
        outFile.close();
        qDebug() << "Compressed data written to" << outName;
    }

    // Load and decompress
    if (extract || zlib) {
      return 0;
    }
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
              return 1;
            }
        } else {
            qDebug() << "Decompression failed!";
            return 1;
        }
    }

    return 0;
}
