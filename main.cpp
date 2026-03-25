#include <QByteArray>
#include <QFile>
#include <QDebug>

int main() {
    // Original data
    QFile file("distro.smime");
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
            qDebug() << "Decompressed data:" << decompressedData;
        } else {
            qDebug() << "Decompression failed!";
        }
    }

    return 0;
}   
