/****************************************************************************
**
** DocToText - Converts DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP),
**             OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE),
**             ODFXML (FODP, FODS, FODT), PDF, EML and HTML documents to plain text.
**             Extracts metadata and annotations.
**
** Copyright (c) 2006-2013, SILVERCODERS(R)
** http://silvercoders.com
**
** Project homepage: http://silvercoders.com/en/products/doctotext
**
** This program may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file COPYING.GPL included in the
** packaging of this file.
**
** Please remember that any attempt to workaround the GNU General Public
** License using wrappers, pipes, client/server protocols, and so on
** is considered as license violation. If your program, published on license
** other than GNU General Public License version 2, calls some part of this
** code directly or indirectly, you have to buy commercial license.
** If you do not like our point of view, simply do not use the product.
**
** Licensees holding valid commercial license for this product
** may use this file in accordance with the license published by
** SILVERCODERS and appearing in the file COPYING.COM
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/

#include <fstream>
#include "misc.h"
#include "doctotext_unzip.h"
#include <iostream>
#include "metadata.h"
#include "plain_text_extractor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef DEBUG
	#include "tracing.h"
#endif
#include "../version.h"
using namespace std;

static void version()
{
	printf("DocToText v%s\nConverts DOC, XLS, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX) and HTML documents to plain text\nCopyright (c) 2006-20112 SILVERCODERS(R)\nhttp://silvercoders.com\n", VERSION);
}

static void help()
{
	version();
	printf("\nUsage: doctotext [OPTION]... [FILE]\n\n");
	printf("FILE\t\tname of file to convert\n\n");
	printf("Options:\n");
	printf("--meta\textract metadata instead of text.\n");
	printf("--rtf\ttry to parse as RTF document first.\n");
	printf("--odf\ttry to parse as ODF/OOXML document first.\n");
	printf("--ooxml\ttry to parse as ODF/OOXML document first.\n");
	printf("--xls\ttry to parse as XLS document first.\n");
	printf("--xlsb\ttry to parse as XLSB document first.\n");
	printf("--iwork\ttry to parse as iWork document first.\n");
	printf("--ppt\ttry to parse as PPT document first.\n");
	printf("--doc\ttry to parse as DOC document first.\n");
	printf("--html\ttry to parse as HTML document first.\n");
	printf("--pdf\ttry to parse as PDF document first.\n");
	printf("--eml\ttry to parse as EML document first.\n");
	printf("--odfxml\ttry to parse as ODFXML (Open Document Flat XML) document first.\n");
	printf("--fix-xml\ttry to fix corrupted xml files (ODF, OOXML)\n");
	printf("--strip-xml\tstrip xml tags instead of parsing them (ODF, OOXML)\n");
	printf("--unzip-cmd=[COMMAND]\tuse COMMAND to unzip files from archives (ODF, OOXML)\n"
		"\tinstead of build-in decompression library.\n"
		"\t%%d in the command is substituted with destination directory path\n"
		"\t%%a in the command is substituted with name of archive file\n"
		"\t%%f in the command is substituted with name of file to extract\n"
		"\tExample: --unzip-cmd=\"unzip -d %%d %%a %%f\"\n"
	);
	printf("--verbose\tturn on verbose logging\n");
	printf("--log-file=[PATH]\twrite logs to specified file.\n");
}

int main(int argc, char* argv[])
{
	#ifdef DEBUG
		doctotext_init_tracing("doctotext.trace");
	#endif

	if (argc < 2)
	{
		help();
		return EXIT_FAILURE;
	}

	std::string arg;
	std::string cmd;

	bool extract_metadata = false;

	PlainTextExtractor::ParserType parser_type = PlainTextExtractor::PARSER_AUTO;
	XmlParseMode mode = PARSE_XML;

	FormattingStyle options;
	options.table_style = TABLE_STYLE_TABLE_LOOK;
	options.list_style.setPrefix(" * ");
	options.url_style = URL_STYLE_UNDERSCORED;

	bool verbose = false;
	ofstream* log_stream = NULL;

	for(int i = 1 ; i < argc ; i ++)
	{
		arg = argv[i-1];

		if (arg.find("--meta", 0) != -1)
			extract_metadata = true;
		if (arg.find("--rtf", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_RTF;
		if (arg.find("--odfxml", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_ODFXML;
		if (arg.find("--odf", 0) != -1 || arg.find("ooxml", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_ODF_OOXML;
		if (arg.find("--xls", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_XLS;
		if (arg.find("--xlsb", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_XLSB;
		if (arg.find("--iwork", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_IWORK;
		if (arg.find("--ppt", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_PPT;
		if (arg.find("--doc", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_DOC;
		if (arg.find("--html", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_HTML;
		if (arg.find("--pdf", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_PDF;
		if (arg.find("--eml", 0) != -1)
			parser_type = PlainTextExtractor::PARSER_EML;

		if(arg.find("table-style=", 0) != -1)
		{
			if(arg.find("one-row", arg.find("table-style=", 0) + 11) != -1)
			{
				options.table_style = TABLE_STYLE_ONE_ROW;
			}
			if(arg.find("one-col", arg.find("table-style=", 0) + 11) != -1)
			{
				options.table_style = TABLE_STYLE_ONE_COL;
			}
			if(arg.find("table-look", arg.find("table-style=", 0) + 11) != -1)
			{
				options.table_style = TABLE_STYLE_TABLE_LOOK;
			}
		}
		if(arg.find("url-style=", 0) != -1)
		{
			if(arg.find("text-only", arg.find("url-style=", 0) + 10) != -1)
			{
				options.url_style = URL_STYLE_TEXT_ONLY;
			}
			if(arg.find("extended", arg.find("url-style=", 0) + 10) != -1)
			{
				options.url_style = URL_STYLE_EXTENDED;
			}
			if(arg.find("underscored", arg.find("url-style=", 0) + 10) != -1)
			{
				options.url_style = URL_STYLE_UNDERSCORED;
			}
		}
		if(arg.find("list-style-prefix=", 0) != -1)
		{
			options.list_style.setPrefix(arg.substr(arg.find("list-style-prefix=", 0) + 18));
		}
		if (arg.find("fix-xml", 0) != std::string::npos)
			mode = FIX_XML;
		if (arg.find("strip-xml", 0) != std::string::npos)
			mode = STRIP_XML;
		if (arg.find("unzip-cmd=", 0) != -1)
		{
			DocToTextUnzip::setUnzipCommand(arg.substr(arg.find("unzip-cmd=", 0) + 10));
		}
		if (arg.find("verbose", 0) != std::string::npos)
			verbose = true;
		if (arg.find("log-file=", 0) != std::string::npos)
			log_stream = new ofstream(arg.substr(arg.find("log-file=", 0) + 9).c_str());
	}

	PlainTextExtractor extractor(parser_type);
	if (verbose)
		extractor.setVerboseLogging(true);
	if (log_stream != NULL)
		extractor.setLogStream(*log_stream);
	if (mode != PARSE_XML)
		extractor.setXmlParseMode(mode);
	extractor.setFormattingStyle(options);
	if (extract_metadata)
	{
		Metadata meta;
		if (!extractor.extractMetadata(argv[argc - 1], meta))
		{
			(log_stream != NULL ? *log_stream : cerr) << "Error processing file " << argv[argc - 1] << ".\n";
			return EXIT_FAILURE;
		}
		cout << "Author: " << meta.author() << (meta.authorType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nCreation time: " << date_to_string(meta.creationDate()) << (meta.creationDateType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nLast modified by: " << meta.lastModifiedBy() << (meta.lastModifiedByType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nLast modification time: " << date_to_string(meta.lastModificationDate()) << (meta.lastModificationDateType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nPage count: " << meta.pageCount() << (meta.pageCountType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\nWord count: " << meta.wordCount() << (meta.wordCountType() == Metadata::ESTIMATED ? " (estimated)" : "")
			<< "\n";
	}
	else
	{
		string text;
		if (!extractor.processFile(argv[argc - 1], text))
		{
			(log_stream != NULL ? *log_stream : cerr) << "Error processing file " << argv[argc - 1] << ".\n";
			return EXIT_FAILURE;
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
	if (log_stream != NULL)
		delete log_stream;
	return EXIT_SUCCESS;
}
