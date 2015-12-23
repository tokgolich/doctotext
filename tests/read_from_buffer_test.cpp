#include "../build/metadata.h"
#include "../build/plain_text_extractor.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <time.h>

using namespace doctotext;

static std::string date_to_string(const tm& date)
{
	if (date.tm_year == 0)
		return "";
	char buf[64];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &date);
	return buf;
}

int main(int argc, char* argv[])
{
	FormattingStyle options;
	options.table_style = TABLE_STYLE_TABLE_LOOK;
	options.list_style.setPrefix(" * ");
	options.url_style = URL_STYLE_UNDERSCORED;
	bool extract_metadata = false;
	std::string file_name(argv[1]);
	if (argc > 2 && std::string(argv[2]) == "--meta")
		extract_metadata = true;
	std::ifstream src(file_name.c_str(), std::ios_base::in|std::ios_base::binary);
	if (!src.is_open())
	{
		std::cout << "Cannot open a file\n";
		return 1;
	}
	std::string content((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());
	PlainTextExtractor extractor;
	extractor.setFormattingStyle(options);
	if (extract_metadata)
	{
		Metadata meta;
		if (!extractor.extractMetadata(content.c_str(), content.length(), meta))
		{
			std::cerr << "Error processing file " << file_name << ".\n";
			return 1;
		}
		std::cout << "Author: " << meta.author() << (meta.authorType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nCreation time: " << date_to_string(meta.creationDate()) << (meta.creationDateType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nLast modified by: " << meta.lastModifiedBy() << (meta.lastModifiedByType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nLast modification time: " << date_to_string(meta.lastModificationDate()) << (meta.lastModificationDateType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nPage count: " << meta.pageCount() << (meta.pageCountType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nWord count: " << meta.wordCount() << (meta.wordCountType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\n";
	}
	else
	{
		std::string text;
		if (!extractor.processFile(content.c_str(), content.length(), text))
		{
			std::cerr << "Error processing file " << file_name << ".\n";
			return 1;
		}
		printf("%s\n", text.c_str());
		std::vector<Link> links;
		extractor.getParsedLinks(links);
		if (links.size() > 0)
		{
			printf("parsed links:\n");
			for (size_t i = 0; i < links.size(); ++i)
			{
				printf("%s @ index = %d length = %d\n", links[i].getLinkUrl(), links[i].getLinkTextPosition(), strlen(links[i].getLinkText()));
			}
		}
		std::vector<Attachment> attachments;
		extractor.getAttachments(attachments);
		if (attachments.size() > 0)
		{
			printf("parsed attachments:\n");
			for (size_t i = 0; i < attachments.size(); ++i)
			{
				printf("\nname: %s\n", attachments[i].filename());
				std::map<std::string, Variant> variables = attachments[i].getFields();
				for (std::map<std::string, Variant>::iterator it = variables.begin(); it != variables.end(); ++it)
				{
					#warning TODO: If Content-ID is not present in the file, mimetic generates it... and test/Makefile always goes wrong.\
					Maybe we should skip this field?
					if (it->first != "Content-ID")
						printf("field: %s, value: %s\n", it->first.c_str(), it->second.getString());
				}
			}
		}
	}
	return 0;
}
