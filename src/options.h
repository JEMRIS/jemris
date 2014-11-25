/*
 *  codeare Copyright (C) 2007-2010 Kaveh Vahedipour
 *                               Forschungszentrum Juelich, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */

#ifndef _ANYOPTION_H
#define _ANYOPTION_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#define COMMON_OPT 	            1
#define COMMAND_OPT 	        2
#define FILE_OPT 	            3
#define COMMON_FLAG 	        4
#define COMMAND_FLAG 	        5
#define FILE_FLAG 	            6

#define COMMAND_OPTION_TYPE  	1
#define COMMAND_FLAG_TYPE 	    2
#define FILE_OPTION_TYPE  	    3
#define FILE_FLAG_TYPE 		    4 
#define UNKNOWN_TYPE 		    5

#define DEFAULT_MAXOPTS 	   10
#define MAX_LONG_PREFIX_LENGTH 	2

#define DEFAULT_MAXUSAGE	    3
#define DEFAULT_MAXHELP        10	

#define TRUE_FLAG "true" 

using namespace std;

/**
 * @brief Facility for command line argument parsing and handling
 */
class Options {


 public: 

	/**
	 * @brief Construct and run init with default maximum number of options.
	 */
	Options();

	/**
	 * @brief Construct and run init with given maximum number of options.
	 *
	 * @param maxoptions Maximum number of options.
	 */
    Options(int maxoptions ); 

    /**
     * @brief Construct and run init with given maximum numbers of options 
     */
    Options(int maxoptions , int maxcharoptions); 

    /**
     * @brief Destructor
     *
     * Clean up memory and destroy.
     */
    ~Options();

    /** 
     * following set methods specifies the  
     * special characters and delimiters 
     * if not set traditional defaults will be used
     */

    /**
     * @brief Set command prefix char (i.e. '-' in "-w")
     * 
     * @param _prefix Prefix character.
     */
    void setCommandPrefixChar  (char  _prefix    );
    
    /**
     * @brief Set command long prefix string (i.e. '--' in "--width")
     *
     * @param _prefix Prefix string.
     */
    void setCommandLongPrefix  (char* _prefix    );
    
    /**
     * @brief Set file comment character (i.e. '#' in shell scripts)
     *
     * @param _comment Prefix character.
     */
    void setFileCommentChar    (char  _comment   );

    /**
     * @brief Set file delimiter character (i.e. ':' in "width : 100" )
     *
     * @param _delimiter Delimiter character.
     */
    void setFileDelimiterChar  (char  _delimiter );

    /**
     * provide the input for the options
     * like argv[] for commndline and the 
     * option file name  to use;
     */
    
    
    /**
     * @brief Set command line arguments given
     *
     * @param _argc Number of arguments.
     * @param _argv Arguments.
     */
    void useCommandArgs( int _argc, char **_argv );

    /**
     * @brief Read in configuration file
     *
     * @param _filename Configuration file name.
     */
    void useFileName( const char *_filename );
    
    /** 
     * turn off the POSIX style options 
     * this means anything starting with a '-' or "--"
     * will be considered a valid option 
     * which alo means you cannot add a bunch of 
     * POIX options chars together like "-lr"  for "-l -r"
     * 
     */
    
    /**
     * @brief Non-POSIX input
     */
    void noPOSIX();

    /**
     * @brief Enable verbose printing
     */
    void setVerbose();

    
    /**
     * there are two types of options  
     *
     * Option - has an associated value ( -w 100 )
     * Flag  - no value, just a boolean flag  ( -nogui )
     * 
     * the options can be either a string ( GNU style )
     * or a character ( traditional POSIX style ) 
     * or both ( --width, -w )
     *
     * the options can be common to the commandline and 
     * the optionfile, or can belong only to either of 
     * commandline and optionfile
     *
     * following set methods, handle all the aboove 
     * cases of options.
     */
    
    /* options comman to command line and option file */


    /**
     * @brief Add new option with a given name.
     *
     * @param opt_string Desired name.
     */
    void setOption (const char *opt_string);
    
    /**
     * @brief Add new option with a given character.
     *
     * @param opt_char Desired character.
     */
    void setOption (char        opt_char);
    
    /**
     * @brief Add new option with name 
     *
     * @param opt_string Name.
     * @param opt_char   Character.
     */
    void setOption (const char *opt_string, char opt_char );

    /**
     * @brief Add new flag with a given name.
     *
     * @param opt_string Desired name.
     */
    void setFlag   (const char *opt_string);

    /**
     * @brief Add new flag with a given character.
     *
     * @param opt_char Desired character.
     */
    void setFlag   (char        opt_char);

    /**
     * @brief Add new flag with name 
     *
     * @param opt_string Name.
     * @param opt_char   Character.
     */
    void setFlag            (const char *opt_string, char opt_char);
    
    /* options read from commandline only */
	
    /**
     * @brief Add new option with a given name.
     *
     * @param opt_string Desired name.
     */
    void setCommandOption   (const char *opt_string);

    /**
     * @brief Add new option with a given character.
     *
     * @param opt_char Desired character.
     */
    void setCommandOption   (char  opt_char);

    /**
     * @brief Add new option with name 
     *
     * @param opt_string Name.
     * @param opt_char   Character.
     */
    void setCommandOption   (const char *opt_string, char opt_char);

    /**
     * @brief Add new flag with a given name.
     *
     * @param opt_string Desired name.
     */
    void setCommandFlag     (const char *opt_string);

    /**
     * @brief Add new flag with a given character.
     *
     * @param opt_char Desired character.
     */
    void setCommandFlag     (char  opt_char);

    /**
     * @brief Add new flag with name 
     *
     * @param opt_string Name.
     * @param opt_char   Character.
     */
    void setCommandFlag     (const char *opt_string, char opt_char);
    
    /* options read from an option file only  */

    /**
     * @brief Add new option with a given name.
     *
     * @param opt_string Desired name.
     */
    void setFileOption      (const char *opt_string);

    /**
     * @brief Add new option with a given character.
     *
     * @param opt_char Desired character.
     */
    void setFileOption      (char  opt_char);

    /**
     * @brief Add new option with name 
     *
     * @param opt_string Name.
     * @param opt_char   Character.
     */
    void setFileOption      (const char *opt_string, char opt_char);

    /**
     * @brief Add new flag with a given name.
     *
     * @param opt_string Desired name.
     */
    void setFileFlag        (const char *opt_string);

    /**
     * @brief Add new flag with a given character.
     *
     * @param opt_char Desired character.
     */
    void setFileFlag        (char  opt_char);

    /**
     * @brief Add new flag with name 
     *
     * @param opt_string Name.
     * @param opt_char   Character.
     */
    void setFileFlag        (const char *opt_string, char opt_char);
    
    /*
     * process the options, registerd using 
     * useCommandArgs() and useFileName();
     */

	/**
	 * @brief Process options
	 */
    void processOptions     ();  

	/**
	 * @brief Process command arguments
	 */
    void processCommandArgs ();

	/**
	 * @brief Process command arguments to a maximum number.
	 *
	 * @param max_args Maximum number of arguments processed.
	 */
    void processCommandArgs (int max_args);

	/**
	 * @brief Process options
	 */
    bool processFile        ();
    
    /*
     * process the specified options 
     */

	/**
	 * @brief Process command arguments given.
	 *
	 * @param _argc Number of arguments.
	 * @param _argv Arguments.
	 */
    void processCommandArgs (int _argc, char** _argv);

	/**
	 * @brief Process command arguments given to a given maximum number.
	 *
	 * @param _argc    Number of arguments.
	 * @param _argv    Arguments.
	 * @param max_args Maximum number of arguments processed.
	 */
    void processCommandArgs (int _argc, char** _argv, int max_args);

	/**
	 * @brief Process options file.
	 *
	 * @param _filename Name of the file.
	 */
    bool processFile        (const char* _filename);
    
    /*
     * get the value of the options 
     * will return NULL if no value is set 
     */
	
	/**
	 * @brief Get value of key string.
	 *
	 * @param _option Key.
	 */
    char *getValue          (const char* _option);

	/**
	 * @brief Get flag state of key string.
	 *
	 * @param _option Key.
	 */
    bool  getFlag           (const char* _option);

	/**
	 * @brief Get value of key character.
	 *
	 * @param _optchar Key.
	 */
    char *getValue          (char        _optchar);

	/**
	 * @brief Get flag state of key character.
	 *
	 * @param _optchar Key.
	 */
    bool  getFlag           (char        _optchar);
    
    /*
     * Print Usage
     */
	
	/**
	 * @brief Print usage.
	 */
    void printUsage         ();

	/**
	 * @brief Print auto usage.
	 */
    void printAutoUsage     ();
	
	/**
	 * @brief Append given line to usage.
	 *
	 * @param line Line to be appended.
	 */
    void addUsage           (const char* line);

	/**
	 * @brief Print help screen.
	 */
    void printHelp          ();

    /* print auto usage printing for unknown options or flag */
	/**
	 * @brief Set usage to be printed automatically.
	 */
    void autoUsagePrint     (bool flag);
    
    /* 
     * get the argument count and arguments sans the options
     */
	
	/**
	 * @brief Get number of command line args.
	 */
    int   getArgc           ();

	/**
	 * @brief Get command line args.
	 */
    char* getArgv           (int index);

	/**
	 * @brief Any options at all?
	 */
    bool  hasOptions        ();

    
 private: 
    
    /* the hidden data structure */
    int          argc;                /**< commandline arg count                   **/
    char**       argv;                /**< commndline args                         **/
    const char*  filename;            /**< the option file                         **/
    char*        appname;             /**< the application name from argv[0]       **/
    
    int*         new_argv;            /**< arguments sans options (index to argv)  **/
    int          new_argc;            /**< argument count sans the options         **/
    int          max_legal_args;      /**< ignore extra arguments                  **/
    
    
    /* option strings storage + indexing */
    int          max_options;         /**< maximum number of options               **/
    const char** options;             /**< storage                                 **/
    int*         optiontype;          /**< type - common, command, file            **/
    int*         optionindex;         /**< index into value storage                **/
    int          option_counter;      /**< counter for added options               **/
    
    /* option chars storage + indexing */
    int          max_char_options;    /**< maximum number options                  **/
    char*        optionchars;         /**< storage                                 **/
    int*         optchartype;         /**< type - common, command, file            **/
    int*         optcharindex;        /**< index into value storage                **/
    int          optchar_counter;     /**< counter for added options               **/
    
    /* values */
    char**       values;              /**< common value storage                    **/
    int          g_value_counter;     /**< globally updated value index LAME!      **/
    
    /* help and usage */
    const char** usage;               /**< usage                                   **/
    int          max_usage_lines;     /**< max usage lines reseverd                **/
    int          usage_lines;         /**< number of usage lines                   **/
    
    bool         command_set;         /**< if argc/argv were provided              **/
    bool         file_set;            /**< if a filename was provided              **/
    bool         mem_allocated;       /**< if memory allocated in init()           **/
    bool         posix_style;         /**< enables to turn off POSIX style options **/
    bool         verbose;             /**< silent|verbose                          **/
    bool         print_usage;         /**< usage verbose                           **/
    bool         print_help;          /**< help verbose                            **/
    
    char         opt_prefix_char;     /**<  '-' in "-w"                            **/
    char         long_opt_prefix[MAX_LONG_PREFIX_LENGTH+1]; /* '--' in "--width"     **/
    char         file_delimiter_char; /**< ':' in width : 100                      **/
    char         file_comment_char;   /**<  '#' in "#this is a comment"            **/
    char         equalsign;
    char         comment;
    char         delimiter;
    char         endofline;
    char         whitespace;
    char         nullterminate;
    
    bool         set;                 //was static member
    bool         once;                //was static member
    
    bool         hasoptions;
    bool         autousage;
    
 private: /* the hidden utils */
    void init               ();    
    void init               (int maxopt, int maxcharopt );    
    bool alloc              ();
    void cleanup            ();
    bool valueStoreOK       ();
    
    /* grow storage arrays as required */
    bool doubleOptStorage   ();
    bool doubleCharStorage  ();
    bool doubleUsageStorage ();
    
    bool setValue           (const char *option , char *value );
    bool setFlagOn          (const char *option );
    bool setValue           (char optchar , char *value);
    bool setFlagOn          (char optchar );
    
    void addOption          (const char* option , int type );
    void addOption          (char optchar , int type );
    void addOptionError     (const char *opt);
    void addOptionError     (char opt);
    bool findFlag           (char* value );
    void addUsageError      (const char *line );
    bool CommandSet         ();
    bool FileSet            ();
    bool POSIX              ();

    char parsePOSIX         (char* arg );
    int parseGNU            (char *arg );
    bool matchChar          (char c );
    int matchOpt            (char *opt );

    /* dot file methods */
    char *readFile          ();
    char *readFile          (const char* fname );
    bool consumeFile        (char *buffer );
    void processLine        (char *theline, int length );
    char *chomp             (char *str );
    void valuePairs         (char *type, char *value ); 
    void justValue          (char *value );

    void printVerbose       (const char *msg );
    void printVerbose       (char *msg );
    void printVerbose       (char ch );
    void printVerbose       ();


};

#endif /* ! _ANYOPTION_H */
