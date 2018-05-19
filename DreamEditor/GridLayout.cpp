#include "GridLayout.h"

// Pending Adding Support for padding. Notice that padding only afffect inner table distance, not the outer sides, which should be controlled by the interface by assigning origins
AutoExpandGridLayout::AutoExpandGridLayout(int originX, int originY, unsigned int inRowPadding, unsigned int inColumnPdding)	// The coordiantes are used to transform(i.e. Model Matrix) the Interface Elements
// Rememebr that when generating renderables for the interface elements, they are all begin at the openGL origin, which means not transformation dependant
:beginOffsetX(originX), beginOffsetY(originY), rowPadding(inRowPadding), columnPadding(inColumnPdding)
{}

void AutoExpandGridLayout::AddToGrid(InterfaceElement* element, GridAlignment alignment)
{
	// Part1: Element Addition
	// If rows are empty now, create one
	if (rows.Last() == NULL)
	{
		Row *newRow = new Row();
		rows.Add(newRow);

		Grid* newGrid = new Grid();
		newRow->grids.Add(newGrid);

		newGrid->elements.Add(element);
		newGrid->alignment = alignment;
		newGrid->gridWidth += element->GetDimensionX();
		if (element->GetDimensionY() > newGrid->gridHeight)
		{
			newGrid->gridHeight = element->GetDimensionY();
		}
	}
	else
	{
		// Add element to the current row, current grid
		Row* currentRow = (Row*)(rows.Last());
		Grid* currentGrid = (Grid*)(currentRow->grids.Last());
		currentGrid->elements.Add(element);
		currentGrid->alignment = alignment;
		currentGrid->gridWidth += element->GetDimensionX();
		if (element->GetDimensionY() > currentGrid->gridHeight)
		{
			currentGrid->gridHeight = element->GetDimensionY();
		}
	}

	// Part 2: Bookkeeping
	// Also Remember to update Important Information
	Row* currentRow = (Row*)(rows.Last());
	int elementHeight = element->GetDimensionY();
	if (elementHeight > currentRow->maxHeight)
	{
		currentRow->maxHeight = elementHeight;
	}
	currentRow->rowWidth += element->GetDimensionX();
}

void AutoExpandGridLayout::AddToNewRow(InterfaceElement* element, GridAlignment alignment)
{
	// Part1: Element Addition
	Row *newRow = new Row();
	rows.Add(newRow);

	Grid* newGrid = new Grid();
	newRow->grids.Add(newGrid);

	newGrid->elements.Add(element);
	newGrid->alignment = alignment;
	newGrid->gridWidth += element->GetDimensionX();
	if (element->GetDimensionY() > newGrid->gridHeight)
	{
		newGrid->gridHeight = element->GetDimensionY();
	}

	// Part 2: Bookkeeping
	// Also Remember to update Important Information
	if (element->GetDimensionY() > newRow->maxHeight)
	{
		newRow->maxHeight = element->GetDimensionY();
	}
	newRow->rowWidth += element->GetDimensionX();
}

void AutoExpandGridLayout::AddToRow(InterfaceElement* element, GridAlignment alignment)
{
	// If the list is empty, then create a new row
	if (rows.Length() == 0)
	{
		AddToNewRow(element, alignment);
	}
	else
	{
		// Part1: Element Addition
		Row* currentRow = (Row*)(rows.Last());

		Grid* newGrid = new Grid();
		currentRow->grids.Add(newGrid);

		newGrid->alignment = alignment;
		newGrid->elements.Add(element);
		newGrid->gridWidth += element->GetDimensionX();
		if (element->GetDimensionY() > newGrid->gridHeight)
		{
			newGrid->gridHeight = element->GetDimensionY();
		}

		// Part 2: Bookkeeping
		// Also Remember to update Important Information
		int elementHeight = element->GetDimensionY();
		if (elementHeight > currentRow->maxHeight)
		{
			currentRow->maxHeight = elementHeight;
		}
		currentRow->rowWidth += rowPadding + element->GetDimensionX();	// Row Padding Here: rowPadding is only added when add to a row a new grid; Not padding in first row grid, and no padding inside grid
	}

}


// Debug: X, Y sign and padding ; Why already padded
void AutoExpandGridLayout::Dispatch()
{
	// Set Final Layout Dimensions
	layoutWidth = 0;
	layoutHeight = -beginOffsetY;

	// First Iterate through all rows to find the layout dimensions: Maximum Row Width and Layout Height
	// Dspatch Space For each row
	Row* currentRow = (Row*)(rows.First());
	while (currentRow)
	{
		// Calculate Dimension
		layoutHeight += currentRow->maxHeight + columnPadding;	// Column Padding Here
		if (currentRow->rowWidth > layoutWidth) { layoutWidth = currentRow->rowWidth; }

		// Continue Next
		currentRow = (Row*)(rows.Next());
	}
	layoutWidth += beginOffsetX * 2;

	// Then go through everything again to layout the elements according to layout width and row height
	// Dspatch Space For each row
	currentRow = (Row*)rows.First();
	int accumuRowHeight = 0;	// Accumulated RowHeights
	while (currentRow)
	{
		// Diapatch Space for each grid
		unsigned int accuRowWidth = 0;	// Effectively Disable Horizontal Alignment

		Grid* currentGrid = (Grid*)(currentRow->grids.First());
		while (currentGrid)
		{
			// Position Grid itself with the table(The whole layout)
			int localx = 0;
			int localy = 0;
			switch (currentGrid->alignment)
			{
				// The caculations below are done assuming Y is posive, which will be negated later when Update Elements transformation
			case Align_Center:
				localx = accuRowWidth;
				localy = accumuRowHeight + currentRow->maxHeight /2 - currentGrid->gridHeight / 2;	// The same as Aligh_Left, Aligh_Right
				break;
			case Align_Up:
				localx = accuRowWidth;
				localy = accumuRowHeight;
				break;
			case Align_Down:
				localx = accuRowWidth;
				localy = accumuRowHeight + (currentRow->maxHeight - currentGrid->gridHeight);
				break;
			default:
			case Align_Left:
				localx = accuRowWidth;
				localy = accumuRowHeight + currentRow->maxHeight / 2 - currentGrid->gridHeight / 2;
				break;
			case Align_Right:
				localx = accuRowWidth;
				localy = accumuRowHeight + currentRow->maxHeight / 2 - currentGrid->gridHeight / 2;
				break;
			case Align_UpLeft:
				localx = accuRowWidth;
				localy = accumuRowHeight;
				break;
			case Align_UpRight:
				localx = accuRowWidth;
				localy = accumuRowHeight;
				break;
			case Align_LowerLeft:
				localx = accuRowWidth;
				localy = accumuRowHeight + (currentRow->maxHeight - currentGrid->gridHeight);
				break;
			case Align_LowerRight:
				localx = accuRowWidth;
				localy = accumuRowHeight + (currentRow->maxHeight - currentGrid->gridHeight);
				break;
			}

			// For each Element in the grid
			InterfaceElement* element = (InterfaceElement*)(currentGrid->elements.First());
			unsigned int accumuElementWidth = 0;
			while (element)
			{
				// Layout the element according to the element size within the grid, fixed linear flow with centering
				int localx2 = accumuElementWidth;
				int localy2 = currentGrid->gridHeight/2 - element->GetDimensionY()/2;	// Effectively center the object

				/// Update Element
				element->UpdatePosition(beginOffsetX + localx + localx2, beginOffsetY - localy - localy2);	// Y is negative since in openGL second quadrant

				// Continue Next
				accumuElementWidth += element->GetDimensionX();
				element = (InterfaceElement*)(currentGrid->elements.Next());
			}

			// Continue Next
			accuRowWidth += currentGrid->gridWidth + rowPadding;	// Row Padding Here; Notice GridWidth is still the actual added widths of elements without padding
			currentGrid = (Grid*)(currentRow->grids.Next());
		}

		// Continue Next
		accumuRowHeight += currentRow->maxHeight + columnPadding;
		currentRow = (Row*)(rows.Next());
	}
}