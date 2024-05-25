#include "scenario.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "strings.h"

JobRequest::JobRequest(Vertex start, Vertex end) : startVertex(start), endVertex(end)
{
}

WeightedDiGraph DefaultScenarioLoader::getGraph() const
{
  DefaultGraphLoader graphLoader;
  return graphLoader.getGraph();
}

std::vector<JobRequest> DefaultScenarioLoader::getjobRequests() const
{
  return {JobRequest(0, 2)};
}

std::string readVersion(
    std::ifstream& file, const std::string expectedParameterTitle, const std::filesystem::path& filename)
{
  std::string line;
  std::getline(file, line);
  std::string label;
  std::string stringValue;
  std::istringstream lineStream(line);
  std::getline(lineStream, label, ' ');
  if (label != expectedParameterTitle)
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "': invalid format. Expected 'type octile', got '" << label << "'";
    throw std::invalid_argument(message.str());
  }
  std::getline(lineStream, stringValue, ' ');
  return stringValue;
}

JobRequest FileScenarioLoader::parseJobRequest(const std::string& line, const std::filesystem::path& filename)
{
  const std::vector<std::string> lineParts = split(line, {' ', '\t'});

  std::string_view mapFilename = lineParts[1];
  if (!graphLoader)
  {
    const auto graphFilenamePath = std::filesystem::path(mapFilename);
    const std::string graphFilename = std::filesystem::path(filename)
                                          .replace_filename(graphFilenamePath.filename())
                                          .replace_extension(graphFilenamePath.extension())
                                          .string();
    graphLoader = std::make_unique<MapGraphLoader>(graphFilename);
  }
  else
  {
    if (std::filesystem::path(mapFilename).filename() != std::filesystem::path(graphLoader->getFilename()).filename())
    {
      std::ostringstream message;
      message << "Failed to open '" << filename << "': Scenario files with multiple maps not supported.";
      throw std::invalid_argument(message.str());
    }
  }

  std::string startXString = std::string(lineParts[4]);
  int startX = std::stoi(startXString);

  std::string startYString = std::string(lineParts[5]);
  int startY = std::stoi(startYString);

  std::string endXString = std::string(lineParts[6]);
  int endX = std::stoi(endXString);

  std::string endYString = std::string(lineParts[7]);
  int endY = std::stoi(endYString);

  const auto startVertex = graphLoader->convertMapPositionToVertexIndex(startY, startX);
  if (!startVertex)
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "': invalid format. Unable to convert map position '" << startX
            << "', '" << startY << "' to vertex index.";
    throw std::invalid_argument(message.str());
  }
  const auto endVertex = graphLoader->convertMapPositionToVertexIndex(endY, endX);
  if (!endVertex)
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "': invalid format. Unable to convert map position '" << startX
            << "', '" << startY << "' to vertex index.";
    throw std::invalid_argument(message.str());
  }

  return JobRequest(*startVertex, *endVertex);
}

FileScenarioLoader::FileScenarioLoader(const std::filesystem::path& filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "' from " << std::filesystem::current_path();
    throw std::runtime_error(message.str());
  }

  /*std::string version =*/readVersion(file, "version", filename);
  // if(version != "1.0") {
  //     std::ostringstream message;
  //     message << "Failed to open '" << filename << "': invalid format. Expected 'version 1.0', got '" << version <<
  //     "'"; throw std::invalid_argument(message.str());
  // }

  std::string line;
  while (file && !file.eof())
  {
    std::getline(file, line);
    if (!line.empty())
    {
      jobRequests.emplace_back(parseJobRequest(line, filename));
    }
  }
}

std::vector<JobRequest> FileScenarioLoader::getjobRequests() const
{
  return jobRequests;
}

WeightedDiGraph FileScenarioLoader::getGraph() const
{
  return graphLoader->getGraph();
}
