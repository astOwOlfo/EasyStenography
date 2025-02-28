#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <optional>

#include "abbreviations.hpp"

void parse_abbreviations_file(Abbreviations& abbreviations, const std::string& filename);

#pragma once

#include "parser.cpp"