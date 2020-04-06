#ifndef POKITTOTUI_PTUI_SYMBOL_HPP
#	define POKITTOTUI_PTUI_SYMBOL_HPP

#	include <cstdint>


namespace ptui
{
	// Special UI symbols.
	enum class Symbol
	{
		empty,
		
		// Arrows
		left,
		right,
		up,
		down,
		
		// Checkbox
		checked,
		unchecked,

		space,		
	};
};


#endif // POKITTOTUI_PTUI_SYMBOL_HPP