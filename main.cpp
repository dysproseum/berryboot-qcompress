#include <QByteArray>
#include <QFile>
#include <QDebug>

int main() {
    // Original data
    QByteArray originalData = "Hello, this is some text to compress.";

    // Compress the data
    QByteArray compressedData = qCompress(originalData, 9); // Use level 9 for maximum compression

    // Save compressed data to a file
    QFile outFile("compressed.bin");
    if (outFile.open(QIODevice::WriteOnly)) {
        outFile.write(compressedData);
        outFile.close();
        qDebug() << "Compressed data written to file.";
    }

    // Load and decompress
    QFile inFile("compressed.bin");
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
