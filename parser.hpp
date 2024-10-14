#ifndef __PARSER_H
#define __PARSER_H

#include "lexer.hpp"
#include <vector>
#include <stack>
#include <unordered_map>

enum NODE_TYPE
{
    NODE_ROOT,
    NODE_VARIABLE,
    NODE_ARRAY,
    NODE_ARRAY_VARIABLE,
    NODE_RETURN,
    NODE_EXIT,
    NODE_PRINT,
    NODE_GET,
    NODE_INT,
    NODE_STRING,
    };

    std::string nodeToString(enum NODE_TYPE TYPE)
{
    switch (TYPE)
    {
        case NODE_ROOT : return "NODE_ROOT";
        case NODE_VARIABLE : return "NODE_VARIABLE";
	    case NODE_ARRAY : return "NODE_ARRAY";
        case NODE_ARRAY_VARIABLE : return "NODE_ARRAY_VARIABLE";
        case NODE_RETURN : return "NODE_RETURN";
        case NODE_EXIT : return "NODE_EXIT";
        case NODE_PRINT : return "NODE_PRINT";
 	    case NODE_GET : return "NODE_GET";
        case NODE_INT : return "NODE_INT";
        case NODE_STRING : return "NODE_STRING";
         }
}
struct AST_NODE 
{
    enum NODE_TYPE TYPE;
    std::string * VALUE;
    AST_NODE * CHILD;
    AST_NODE * LIMIT;
    std::vector <AST_NODE *> SUB_STATEMENTS; 
    std::vector <AST_NODE *> SUB_VALUES;
    AST_NODE * SUPPLEMENT; 	
    bool REFERENCE;    
};
class Parser{
    public:
    Parser(std::vector <Token *> tokens)
    {
        parserTokens = tokens;
        index = 0;
        limit = parserTokens.size();
        current = parserTokens.at(index);
    }

    std::unordered_map <std::string , enum NODE_TYPE> operatorMap = {
        {"+" ,     NODE_OP_ADD},
        {"-" ,     NODE_OP_SUB},
        {"*" ,     NODE_OP_MUL},
        {"/" ,     NODE_OP_DIV},
        {"%" ,     NODE_OP_MOD},
	    {"plus" ,  NODE_OP_ADD},
	    {"minus" , NODE_OP_SUB},
	    {"into" ,  NODE_OP_MUL},
        {"mod" ,   NODE_OP_MOD},
        {"times" , NODE_OP_MUL},
        {"by" ,    NODE_OP_DIV},
        
    };
};
std::vector <std::string> colorSupport = {
        "black",
        "red" ,
        "green" ,
        "yellow" ,
        "blue" , 
        "purple" ,
        "cyan" ,
        "white",
        "background_black",
        "background_red" ,
        "background_green" ,
        "background_yellow" ,
        "background_blue" , 
        "background_purple" ,
        "background_cyan" ,
        "background_white",
    };

    std::vector <std::string> displayUtils = {
        "inline"
    };

     Token * tokenSeek (int offset)
    {
	if (index + offset < limit)
		return parserTokens.at(index+offset);
	else
		return parserTokens.at(limit-1); 

    Token * proceed (enum type TYPE) 
    {
        if (current->TYPE != TYPE)
        {
            std::cout << "[!] SYNTAX ERROR ! Expected " << typeToString(TYPE) << " but got : " << typeToString(current->TYPE) << std::endl;
            exit(1);
        } 
        index++;
        current = parserTokens.at(index);
        return current;
    }

    void subParseINT_ID(std::vector<AST_NODE *> &newNodeVector)
    {
        switch (current->TYPE)
		    {
			    case TOKEN_INT : {newNodeVector.push_back(parseINT()); break;}
            	case TOKEN_ID  : {newNodeVector.push_back(parseID_RHS()); break;}
            	default        : {raiseSyntaxError(4);}
		    }
    }

    void subParseINT_ID_MATH(std::vector<AST_NODE *> &newNodeVector)
    {
        switch(current->TYPE)
		{
		    case TOKEN_INT : {newNodeVector.push_back(parseINT()); break;}
		    case TOKEN_ID : {newNodeVector.push_back(parseID_RHS()); break;}	  
		    case TOKEN_MATH : 
		    {
			    if (current->VALUE != "-")
			    {
				    std::cout << "[!] Unsupported mathematical operator while creating the array : " << current->VALUE; 
				    exit(1);
			    }
			    newNodeVector.push_back(parseINT(true));
			    break;
		    } 
		    default : {raiseSyntaxError(5);}
		    
		}
    }
    void raiseSyntaxError(int errorCode = 0) 
    {
        if (errorCode != 0)
            std::cout << errorTable[errorCode-1];
        else
            std::cout << "[!] Syntax Error !" << std::endl;

        exit(1);
    }

    void raiseUnidentifiedTokenError()
    {
        std::cout << "[!] Syntax Error ! Unindentified Token : " << typeToString(current->TYPE) << std::endl;
        exit(1);
    }

    void raiseUnexpectedTokenError()
    {
        std::cout << "[!] Syntax Error ! Unexpected Token : " << typeToString(current->TYPE) << std::endl;
        exit(1);
    }

    void handleIndentation() 
    { 
        bufferScope = stoi (current->VALUE);
	    if (bufferScope <= scopeLog.top())
	    {
		    std::cout << "[!] Indentation Error : Expected further indentation " << std::endl;
		    exit(1);
	    }
	    scopeLog.push(bufferScope);
    }

    void inScopeParsing(AST_NODE * newNode) 
    {
        proceed(TOKEN_SEMICOLON);
	    switch (current->TYPE)
            {
            case TOKEN_ID : 
		    {
                if (tokenSeek(1)->TYPE == TOKEN_ARE)
			        newNode->SUB_STATEMENTS.push_back(parse1D_ARRAY());
		        else if (tokenSeek(1)->TYPE == TOKEN_EQUALS && tokenSeek(2)->TYPE == TOKEN_REL_LESSTHAN)
			        newNode->SUB_STATEMENTS.push_back(parseARRAY());
		        else if (tokenSeek(1)->TYPE == TOKEN_LEFT_SQR)
			        newNode->SUB_STATEMENTS.push_back(parseARRAY_VARIABLE());
                else 
			        newNode->SUB_STATEMENTS.push_back(parseID()); 
		        break;
		    }
            case TOKEN_KEYWORD : {newNode->SUB_STATEMENTS.push_back(parseKEYWORD()); break;}
	        case TOKEN_RANGE   : {newNode->SUB_STATEMENTS.push_back(parseRANGE()); break;} 
		    case TOKEN_TILL    : {newNode->SUB_STATEMENTS.push_back(parseTILL()); break;}
            case TOKEN_CALL    : {newNode->SUB_STATEMENTS.push_back(parseCALL()); break;}
            default            : {raiseSyntaxError();}
            }
            if (current->TYPE != TOKEN_SEMICOLON)
                raiseUnexpectedTokenError();
    }

    AST_NODE * parseINT(bool negativeNumber = false) 
    {
        if (negativeNumber)
            proceed(TOKEN_MATH);
        if (current->TYPE != TOKEN_INT)
            raiseSyntaxError();

        AST_NODE * newNode = new AST_NODE();
        newNode->TYPE = NODE_INT;
        if (negativeNumber)
	    {
		    int toNegate = std::stoi(current->VALUE);
            toNegate *= -1;
		    current->VALUE = std::to_string(toNegate);
	    }
        newNode->VALUE = &current->VALUE;
        proceed(TOKEN_INT);
        return newNode;
    }

    AST_NODE * parseID_RHS(bool isReference = false) 
    {
        std::string * buffer = &current->VALUE;
        proceed(TOKEN_ID);

        AST_NODE * newNode = new AST_NODE();
        
        newNode->VALUE = buffer;
        newNode->TYPE = NODE_VARIABLE;
        
	    if (isReference)
		    newNode->REFERENCE = isReference;
        
        return newNode;
    }

    AST_NODE * parseOPERATOR() 
    {
        AST_NODE * newNode = new AST_NODE();
        newNode->TYPE = operatorMap[current->VALUE];
        proceed(TOKEN_MATH);
        return newNode;
    }

    AST_NODE * parseCALL() 
    {
        proceed(TOKEN_CALL);
        AST_NODE * newNode = new AST_NODE();
        newNode->TYPE = NODE_CALL;
        newNode->CHILD = parseID_RHS();

	    if (current->TYPE == TOKEN_ARGUMENTS)
	    {
	        proceed(TOKEN_ARGUMENTS);
	        while (true)
	        {    
		        switch (current->TYPE)
		        {
			        case TOKEN_INT : {newNode->SUB_VALUES.push_back(parseINT()); break;}
		            case TOKEN_ID  : {newNode->SUB_VALUES.push_back(parseID_RHS()); break;}
                    case TOKEN_REFERENCE : 
                    {
                        proceed(TOKEN_REFERENCE);
                        newNode->SUB_VALUES.push_back(parseID_RHS(true)); 
                        break;
                    }
		            default : {raiseSyntaxError(2);}
		        }
		        if (current->TYPE != TOKEN_COMMA)
			        break;
		        proceed(TOKEN_COMMA);
	        }
	    }
        return newNode;
    }

    AST_NODE * parseFUNCTION() 
    {
        proceed(TOKEN_FUNCTION);
        AST_NODE * newNode = new AST_NODE();
        newNode->TYPE = NODE_FUNCTION;
        newNode->CHILD = parseID_RHS();

        if (current->TYPE == TOKEN_ARGUMENTS)
        {
	        proceed(TOKEN_ARGUMENTS);
	        while (true)
	        {
		        if (current->TYPE != TOKEN_ID && current->TYPE != TOKEN_REFERENCE )
                    raiseSyntaxError(3);	       
		        if (current->TYPE == TOKEN_REFERENCE)
		        {
			        proceed(TOKEN_REFERENCE);
   			        newNode->SUB_VALUES.push_back(parseID_RHS(true));
		        }
		        else
		            newNode->SUB_VALUES.push_back(parseID_RHS());

		        if (current->TYPE != TOKEN_COMMA)
			        break; 
		        proceed(TOKEN_COMMA);
	        }
        }
    
        proceed(TOKEN_INDENT);
	    if (current->TYPE != TOKEN_SEMICOLON)
            raiseSyntaxError();
	    
	    handleIndentation();
	    
	    while (std::stoi(current->VALUE) == scopeLog.top())
            inScopeParsing(newNode);
	
	    scopeLog.pop();
        return newNode;
    }

    AST_NODE * parseMATH()
    {
    	AST_NODE * newNode = new AST_NODE();
    	newNode->TYPE = NODE_MATH;

    	subParseINT_ID(newNode->SUB_STATEMENTS);
        newNode->SUB_STATEMENTS.push_back(parseOPERATOR());
    	subParseINT_ID(newNode->SUB_STATEMENTS);

        while (current->TYPE == TOKEN_MATH)
        {
            newNode->SUB_STATEMENTS.push_back(parseOPERATOR());
    	    subParseINT_ID(newNode->SUB_STATEMENTS);
        }
    	return newNode;
    }
    }
