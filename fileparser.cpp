#include "fileparser.h"

fileParser::fileParser(QObject *parent) :
    QObject(parent)
{
}

void fileParser::clearList()
{
    this->desiredFiles.clear();
}

void fileParser::sepratingFiles(const QString &parameter)
{
    QDir path(parameter);
    QDirIterator it(parameter, QDirIterator::Subdirectories);
    int numberOfFiles = 0;

    while (it.hasNext()) {
        numberOfFiles++;
        QString fileName = it.next();
        if( fileName.endsWith(".runs") )
        {
            qDebug() << fileName;
            int percent = double(numberOfFiles/path.count())*100;
            emit progressResult(fileName,percent);
            desiredFiles.append(fileName);
        }
    }
    emit fetching_finished();
}

void fileParser::processingFiles()
{
    emit processingResult("Processing Starts...",0);

    QFile output_file("output.txt");
    if (!output_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString errorText = "File '";
        errorText.append("output.txt");
        errorText.append("' cannot be created");
        emit processingResult(errorText,0);
    }

    QTextStream out(&output_file);

    for(int i = 0; i < desiredFiles.size(); i++)
    {
        QStringList list1 = desiredFiles.at(i).split("/");
        QString tmp = "File '";
        tmp.append(list1.last());
        tmp.append("' is processing...");
        int percent = ((double)i/desiredFiles.length())*100;
        emit processingResult(tmp,percent);

        QFile file(desiredFiles.at(i));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QString errorText = "File '";
            errorText.append(list1.last());
            errorText.append("' is not available");
            emit processingResult(errorText,0);
            continue;
        }

        emit processingResult("Reading "+list1.last(),percent);
        QTextStream in(&file);
        QString data = in.readAll();
        QStringList textParts = data.split("------------------------------------------------------------------\n");
        QStringList coincidenceMatrix = textParts.last().split("\n\n");

        emit processingResult("Generating mFile for "+list1.last(),percent);

        QString fileName = list1.last();
        fileName.remove(".runs");
        QStringList outputNameLists = fileName.split("-");

        QString problemType, problemSize, modelSize = "1", RTR = "0", clusteringType = "Random", Sporadic = "0";
        QString learningIteration = "1", parentDisplacement = "yes";

        int listIndex = 0;
        problemType = outputNameLists.at(0);
        if( problemType.startsWith("Trap") )
        {
            problemSize = outputNameLists.at(1);
            listIndex = 2;
        }
        else
        {
            problemType.append("-");
            problemType.append(outputNameLists.at(1));
            problemSize = outputNameLists.at(2);
            listIndex = 3;
        }

        if( fileName.contains("RTR") )
            RTR = "1";

        if( fileName.contains("Kmeans") )
            clusteringType = "Kmeans";

        if( fileName.contains("Sporadic") )
            Sporadic = "1";

        for(int j = listIndex; j < outputNameLists.size(); j++)
        {
            QString listItem = outputNameLists.at(j);
            if( listItem.startsWith("MultiModels") )
                modelSize = listItem.mid(11);
            else if( listItem.endsWith("Iterations") )
            {
                int tmpIndex = listItem.indexOf("Iterations");
                learningIteration = listItem.mid(0,tmpIndex);
            }
        }

        out << problemType << " " << problemSize << " " << modelSize << " " << RTR << " " << clusteringType;
        out << " " << Sporadic << " " <<learningIteration << " " << parentDisplacement;

        QString final_population_size, average_algorithm_time, average_model_building_time;
        QString average_model_used_memory, average_fitness_calls, standard_deviation;

        QStringList lines = coincidenceMatrix.at(0).split("\n");

        QString aatLine = lines.at(2);
        QStringList aatLine_parts = aatLine.split(":");
        QString aatLine_part2 = aatLine_parts.at(1);
        QStringList aatLine_part2_parts = aatLine_part2.split(",");
        QString aatLine_part2_part2 = aatLine_part2_parts.at(1);
        average_algorithm_time = aatLine_part2_part2.mid(0, aatLine_part2_part2.indexOf("seconds"));

        QString ambtLine = lines.at(3);
        QStringList ambtLine_parts = ambtLine.split(":");
        QString ambtLine_part2 = ambtLine_parts.at(1);
        QStringList ambtLine_part2_parts = ambtLine_part2.split(",");
        QString ambtLine_part2_part2 = ambtLine_part2_parts.at(1);
        average_model_building_time = ambtLine_part2_part2.mid(0, ambtLine_part2_part2.indexOf("seconds"));

        QString amumLine = lines.at(4);
        QStringList amumLine_parts = amumLine.split(":");
        QString amumLine_part2 = amumLine_parts.at(1);
        average_model_used_memory = amumLine_part2.mid(0, amumLine_part2.indexOf("bytes"));

        QString fpsLine = lines.at(5);
        QStringList fpsLine_parts = fpsLine.split(":");
        final_population_size = fpsLine_parts.at(1);

        QString lastLine = lines.at(7);
        QStringList lastLine_parts = lastLine.split(",");
        QString afcLine = lastLine_parts.first();
        QStringList afcLine_parts = afcLine.split(":");
        average_fitness_calls = afcLine_parts.at(1);
        QString sdcLine = lastLine_parts.last();
        QStringList sdcLine_parts = sdcLine.split(":");
        standard_deviation = sdcLine_parts.at(1);

        out << final_population_size << average_algorithm_time << average_model_building_time << average_model_used_memory;
        out << average_fitness_calls << standard_deviation << "\n";
    }

    QString outputLog = "output.txt";
    outputLog.append(" is created.");
    emit processingResult(outputLog,100);
    output_file.close();
    emit processing_finished();
}
