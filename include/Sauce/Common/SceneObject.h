// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>
#include <Sauce/Common/Event.h>

BEGIN_SAUCE_NAMESPACE

#define PROPAGATE_EVENT for(SceneObject *child : list<SceneObject*>(m_children)) child->onEvent(e)

/**
* \class	SceneObject
*
* \brief	An game object.
*/
class SAUCE_API SceneObject
{
public:
	SceneObject() :
		m_parent(0),
		m_userData(0)
	{
	}

	/**
	* \fn	virtual void Game::onStart(GameEvent*)
	*
	* \brief	An event called after the game has initialized.
	*
	* \param [in,out]	parameter1	If non-null, the first parameter.
	*/

	virtual void onStart(GameEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onEnd(GameEvent*)
	*
	* \brief	An event called as the game is ending.
	*
	* \param [in,out]	parameter1	If non-null, the first parameter.
	*/

	virtual void onEnd(GameEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onTick(TickEvent*)
	*
	* \brief	An event called when the game wants to update the game state.
	*
	* \param [in,out]	parameter1	If non-null, the first parameter.
	*/

	virtual void onTick(TickEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void SceneObject::onDraw(DrawEvent *e)
	*
	* \brief	An event called when the game wants to draw the current game state.
	*
	* \param [in,out]	e	The event parameters.
	*/

	virtual void onDraw(DrawEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onStepBegin(StepEvent*)
	*
	* \brief	An event called at the start of the game loop.
	*
	* \param [in,out]	parameter1	If non-null, the first parameter.
	*/

	virtual void onStepBegin(StepEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onStepEnd(StepEvent*)
	*
	* \brief	An event called at the end of the game loop.
	*
	* \param [in,out]	parameter1	If non-null, the first parameter.
	*/

	virtual void onStepEnd(StepEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onWindowSizeChanged(WindowEvent*)
	*
	* \brief	Executes the size changed action.
	*
	* \param [in,out]	parameter1	If non-null, the first parameter.
	*/

	virtual void onWindowSizeChanged(WindowEvent *e)
	{
		PROPAGATE_EVENT;
	}

	virtual void onInputEvent(InputEvent *e)
	{
		switch(e->getType())
		{
			case (uint32)KeyEventType::Down:
			case (uint32)KeyEventType::Up:
			case (uint32)KeyEventType::Repeat: onKeyEvent(static_cast<KeyEvent*>(e)); break;
			case (uint32)MouseEventType::Up:
			case (uint32)MouseEventType::Down:
			case (uint32)MouseEventType::Move:
			case (uint32)MouseEventType::Wheel: onMouseEvent(static_cast<MouseEvent*>(e)); break;
			case (uint32)ControllerButtonEventType::Up:
			case (uint32)ControllerButtonEventType::Down:
			case (uint32)ControllerButtonEventType::Repeat:
			case (uint32)CoreEventType::ControllerAxis: onControllerEvent(static_cast<InputEvent*>(e)); break;
		}
	}

	/**
	* \fn	virtual void EventObject::onKeyEvent(KeyEvent *e)
	*
	* \brief	Executes the key up action.
	*
	* \param [in,out]	e	If non-null, the KeyEvent to process.
	*/

	virtual void onKeyEvent(KeyEvent *e)
	{
		switch(e->getType())
		{
			case (uint32)KeyEventType::Down: onKeyDown(e); break;
			case (uint32)KeyEventType::Up: onKeyUp(e); break;
			case (uint32)KeyEventType::Repeat: onKeyRepeat(e); break;
		}
	}

	virtual void onTextInput(TextEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void EventObject::onKeyDown(KeyEvent *e)
	*
	* \brief	Executes the key down action.
	*
	* \param [in,out]	e	If non-null, the KeyEvent to process.
	*/

	virtual void onKeyDown(KeyEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void EventObject::onKeyUp(KeyEvent *e)
	*
	* \brief	Executes the key up action.
	*
	* \param [in,out]	e	If non-null, the KeyEvent to process.
	*/

	virtual void onKeyUp(KeyEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void EventObject::onKeyRepeat(KeyEvent *e)
	*
	* \brief	Executes the key down action.
	*
	* \param [in,out]	e	If non-null, the KeyEvent to process.
	*/

	virtual void onKeyRepeat(KeyEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void SceneObject::onMouseEvent(MouseEvent *e)
	*
	* \brief	Executes the mouse event action.
	*
	* \param [in,out]	e	If non-null, the MouseEvent to process.
	*/

	virtual void onMouseEvent(MouseEvent *e)
	{
		switch(e->getType())
		{
			case (uint32)MouseEventType::Up: onMouseUp(e); break;
			case (uint32)MouseEventType::Down: onMouseDown(e); break;
			case (uint32)MouseEventType::Move: onMouseMove(e); break;
			case (uint32)MouseEventType::Wheel: onMouseWheel(e); break;
		}
	}

	/**
	* \fn	virtual void Game::onMouseMove(MouseEvent *e)
	*
	* \brief	Executes the mouse move action.
	*
	* \param [in,out]	e	If non-null, the MouseEvent to process.
	*/

	virtual void onMouseMove(MouseEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onMouseDown(MouseEvent *e)
	*
	* \brief	Executes the mouse down action.
	*
	* \param [in,out]	e	If non-null, the MouseEvent to process.
	*/

	virtual void onMouseDown(MouseEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onMouseUp(MouseEvent *e)
	*
	* \brief	Executes the mouse up action.
	*
	* \param [in,out]	e	If non-null, the MouseEvent to process.
	*/

	virtual void onMouseUp(MouseEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void Game::onMouseWheel(MouseEvent *e)
	*
	* \brief	Executes the mouse wheel action.
	*
	* \param [in,out]	e	If non-null, the MouseEvent to process.
	*/

	virtual void onMouseWheel(MouseEvent *e)
	{
		PROPAGATE_EVENT;
	}


	virtual void onControllerEvent(InputEvent *e)
	{
		switch(e->getType())
		{
			case (uint32)ControllerButtonEventType::Down:
			case (uint32)ControllerButtonEventType::Up:
			case (uint32)ControllerButtonEventType::Repeat: onControllerButton(static_cast<ControllerButtonEvent*>(e)); break;
			case (uint32)CoreEventType::ControllerAxis: onControllerAxis(static_cast<ControllerAxisEvent*>(e)); break;
		}
	}

	virtual void onControllerButton(ControllerButtonEvent *e)
	{
		PROPAGATE_EVENT;
	}

	virtual void onControllerAxis(ControllerAxisEvent *e)
	{
		PROPAGATE_EVENT;
	}

	/**
	* \fn	virtual void EventObject::onEvent(Event *event)
	*
	* \brief	Executes the event action.
	*
	* \param [in,out]	event	If non-null, the event.
	*/

	virtual void onEvent(Event *e)
	{
		// Find and call the specific event function
		switch(e->getType())
		{
			case (uint32)GameEventType::Start: onStart(static_cast<GameEvent*>(e)); break;
			case (uint32)GameEventType::End: onEnd(static_cast<GameEvent*>(e)); break;
			case (uint32)CoreEventType::Tick: onTick(static_cast<TickEvent*>(e)); break;
			case (uint32)CoreEventType::Draw: onDraw(static_cast<DrawEvent*>(e)); break;
			case (uint32)StepEventType::Begin: onStepBegin(static_cast<StepEvent*>(e)); break;
			case (uint32)StepEventType::End: onStepEnd(static_cast<StepEvent*>(e)); break;
			case (uint32)CoreEventType::TextInput: onTextInput(static_cast<TextEvent*>(e)); break;
			case (uint32)WindowEventType::SizeChanged: onWindowSizeChanged(static_cast<WindowEvent*>(e)); break;
			case (uint32)KeyEventType::Down:
			case (uint32)KeyEventType::Up:
			case (uint32)KeyEventType::Repeat:
			case (uint32)MouseEventType::Up:
			case (uint32)MouseEventType::Down:
			case (uint32)MouseEventType::Move:
			case (uint32)MouseEventType::Wheel:
			case (uint32)ControllerButtonEventType::Up:
			case (uint32)ControllerButtonEventType::Down:
			case (uint32)ControllerButtonEventType::Repeat:
			case (uint32)CoreEventType::ControllerAxis: onInputEvent(static_cast<InputEvent*>(e)); break;
		}
	}

	/**
	* \fn	void SceneObject::addChildFirst(SceneObject *child)
	*
	* \brief	Adds a child first.
	*
	* \param [in,out]	child	If non-null, the child.
	*/

	void addChildFirst(SceneObject *child)
	{
		if(!child) return;
		m_children.push_front(child);
		child->m_parent = this;
	}

	/**
	* \fn	void SceneObject::addChildLast(SceneObject *child)
	*
	* \brief	Adds a child last.
	*
	* \param [in,out]	child	If non-null, the child.
	*/

	void addChildLast(SceneObject *child)
	{
		if(!child) return;
		m_children.push_back(child);
		child->m_parent = this;
	}

	/**
	* \fn	void SceneObject::removeChild(SceneObject *child)
	*
	* \brief	Removes child.
	*
	* \param [in,out]	child	If non-null, the child.
	*/

	void removeChild(SceneObject *child)
	{
		if(!child) return;
		m_children.remove(child);
	}

	/**
	* \fn	void removeChildFront()
	*
	* \brief	Removes the first child.
	*/

	void removeChildFront()
	{
		if(m_children.empty()) return;
		m_children.pop_front();
	}

	/**
	* \fn	void removeChildLast()
	*
	* \brief	Removes the last child.
	*/

	void removeChildLast()
	{
		if(m_children.empty()) return;
		m_children.pop_back();
	}

	/**
	* \fn	list<SceneObject*> getChildren() const
	*
	* \brief	Gets the children of this item.
	*
	* \return	null if it fails, else the children.
	*/

	list<SceneObject*> getChildren() const
	{
		return m_children;
	}

	/**
	* \fn	SceneObject *getParent() const
	*
	* \brief	Gets the parent of this item.
	*
	* \return	null if it fails, else the parent.
	*/

	SceneObject *getParent() const
	{
		return m_parent;
	}

	/**
	* \fn	void SceneObject::setUserData(void *data)
	*
	* \brief	Sets user data.
	*
	* \param [in,out]	data	If non-null, the data.
	*/

	void setUserData(void *data)
	{
		m_userData = data;
	}

	/**
	* \fn	void SceneObject::*getUserData()
	*
	* \brief	Gets user data.
	*
	* \return	null if it fails, else the user data.
	*/

	void *getUserData()
	{
		return m_userData;
	}

private:
	// TODO: Consider using shared_ptr for SceneObject*
	list<SceneObject*> m_children;
	SceneObject *m_parent;
	void *m_userData;
};

END_SAUCE_NAMESPACE
