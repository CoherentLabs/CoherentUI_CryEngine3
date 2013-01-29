/*jslint browser: true, nomen: true, plusplus: true */

/// @file coherent.js
/// @namespace engine

/// Coherent UI JavaScript interface.
/// The `engine` module contains all functions for communication between the UI and the game / application.
(function (factory) {
	if (typeof module === 'object' && module.exports) {
		module.exports = factory(global, global.engine, false);
	} else {
		window.engine = factory(window, window.engine, true);
	}
})(function (global, engine, hasOnLoad) {
	'use strict';


	/**
	* Event emitter
	*
	* @class Emitter
	*/
	function Emitter() {
		this.events = {};
	}

	function Handler(code, context) {
		this.code = code;
		this.context = context;
	}

	Emitter.prototype._createClear = function (object, name, handler) {
		return function() {
			var handlers = object.events[name];
			if (handlers) {
				var index = handlers.indexOf(handler);
				if (index != -1) {
					handlers.splice(index, 1);
					if (handlers.length === 0) {
						delete object.events[name];
					}
				}
			}
		};
	};

	/// @file coherent.js

	/**
	* Add a handler for an event
	*
	* @method on
	* @param name the event name
	* @param callback function to be called when the event is triggered
	* @param context this binding for executing the handler, defaults to the Emitter
	* @return connection object
	*/
	Emitter.prototype.on = function (name, callback, context) {
		var handlers = this.events[name];

		if (handlers === undefined) {
			handlers = this.events[name] = [];
		}

		var handler = new Handler(callback, context || this);
		handlers.push(handler);
		return { clear: this._createClear(this, name, handler) };
	};

	/**
	* Remove a handler from an event
	*
	* @method off
	* @param name the event name
	* @param callback function to be called when the event is triggered
	* @param context this binding for executing the handler, defaults to the Emitter
	* @return connection object
	*/
	Emitter.prototype.off = function (name, handler, context) {
		var handlers = this.events[name];

		if (handlers !== undefined) {
			context = context || this;

			var index;
			var length = handlers.length;
			for (index = 0; index < length; ++index) {
				var reg = handlers[index];
				if (reg.code == handler && reg.context == context) {
					break;
				}
			}
			if (index < length) {
				handlers.splice(index, 1);
				if (handlers.length === 0) {
					delete this.events[name];
				}
			}
		}
	};

	/**
	* Remove a handler from an event
	*
	* @method off
	* @param name the event name
	* @param callback function to be called when the event is triggered
	* @param context this binding for executing the handler, defaults to the Emitter
	* @return connection object
	*/
	Emitter.prototype.trigger = function (name) {
		var handlers = this.events[name];

		if (handlers !== undefined) {
			var args = Array.prototype.slice.call(arguments, 1);

			handlers.forEach(function (handler) {
				handler.code.apply(handler.context, args);
			});
		}
	};

	Emitter.prototype.merge = function (emitter) {
		var lhs = this.events,
			rhs = emitter.events,
			push = Array.prototype.push,
			events;

		for (var e in rhs) {
			events = lhs[e] = lhs[e] || [];
			push.apply(events, rhs[e]);
		}
	};

	var pending = 'pending';
	var fulfilled = 'fulfilled';
	var broken = 'broken';

	function callAsync(code, context, argument) {
		var async = function () {
			code.call(context, argument);
		};
		setTimeout(async);
	}

	function Promise () {
		this.emitter = new Emitter();
		this.state = pending;
		this.result = null;
	}

	Promise.prototype.resolve = function (result) {
		this.state = fulfilled;
		this.result = result;

		this.emitter.trigger(fulfilled, result);
	};

	Promise.prototype.reject = function (result) {
		this.state = broken;
		this.result = result;

		this.emitter.trigger(broken, result);
	};

	Promise.prototype.success = function (code, context) {
		if (this.state !== fulfilled) {
			this.emitter.on(fulfilled, code, context);
		} else {
			callAsync(code, context || this, this.result);
		}
		return this;
	};

	Promise.prototype.always = function (code, context) {
		this.success(code, context);
		this.otherwise(code, context);
		return this;
	};

	Promise.prototype.otherwise = function (code, context) {
		if (this.state !== broken) {
			this.emitter.on(broken, code, context);
		} else {
			callAsync(code, context || this, this.result);
		}
		return this;
	};

	Promise.prototype.merge = function (other) {
		if (this.state === pending) {
			this.emitter.merge(other.emitter);
		} else {
			var handlers = other.emitter.events[this.state];
			var self = this;
			if (handlers !== undefined) {
				handlers.forEach(function (handler) {
					handler.code.call(handler.context, self.result);
				});
			}
		}
	};

	Promise.prototype.make_chain = function (handler, promise, ok) {
		return function (result) {
			var handlerResult;
			try {
				handlerResult = handler.code.call(handler.context, result);
				if (handlerResult instanceof Promise) {
					handlerResult.merge(promise);
				} else if (this.state === ok) {
					promise.resolve(handlerResult);
				} else {
					promise.reject(handlerResult);
				}
			} catch (error) {
				promise.reject(error);
			}
		};
	};

	function makeDefaultHandler(promise) {
		return function () {
			return promise;
		};
	}

	Promise.prototype.then = function (callback, errback) {
		var promise = new Promise();

		var handler = new Handler(callback || makeDefaultHandler(this), this);

		this.success(this.make_chain(handler, promise, fulfilled), this);

		var errorHandler = new Handler(errback || makeDefaultHandler(this), this);
		this.otherwise(this.make_chain(errorHandler, promise, broken), this);


		return promise;
	};

	var isAttached = engine !== undefined;

	engine = engine || {};

	engine.events = {};
	for (var property in Emitter.prototype) {
		engine[property] = Emitter.prototype[property];
	}

	/// @function engine.on
	/// Register handler for and event
	/// @param {String} name name of the event
	/// @param {Function} callback callback function to be executed when the event has been triggered
	/// @param context *this* context for the function, by default the engine object

	/// @function engine.off
	/// Remove handler for an event
	/// @param {String} name name of the event, by default removes all events
	/// @param {Function} callback the callback function to be removed, by default removes all callbacks for a given event
	/// @param context *this* context for the function, by default all removes all callbacks, regardless of context
	/// @warning Removing all handlers for `engine` will remove some *Coherent UI* internal events, breaking some functionality.

	/// @function engine.trigger
	/// Trigger an event
	/// This function will trigger any C++ handler registered for this event with `Coherent::UI::View::RegisterForEvent`
	/// @param {String} name name of the event
	/// @param ... any extra arguments to be passed to the event handlers
	
	engine._trigger = Emitter.prototype.trigger;
	engine.trigger = function (name) {
		this._trigger.apply(this, arguments);
		if (this.events['all'] !== undefined) {
			var allArguments = ['all'].concat(arguments);
			this._trigger.apply(this, allArguments);
		}
	};

	engine.IsAttached = isAttached;
	engine._BindingsReady = false;
	engine._WindowLoaded = false;
	engine._RequestId = 0;
	engine._ActiveRequests = {};

	if (!isAttached) {
		engine.SendMessage = function (name, id) {
			var args = Array.prototype.slice.call(arguments, 2);
			var deferred = engine._ActiveRequests[id];

			delete engine._ActiveRequests[id];

			args.push(deferred);

			var call = (function (name, args) {
				return function () {
					var mock = engine['Mock_' + name];

					if (mock !== undefined) {
						var callMock = function () {
							mock.apply(engine, args);
						};
						window.setTimeout(callMock, 16);
					}
				};
			}(name, args));

			window.setTimeout(call, 16);
		};

		engine.TriggerEvent = function () {
			var args = Array.prototype.slice.call(arguments),
				trigger;

			args[0] = 'Fake_' + args[0];

			trigger = (function (args) {
				return function () {
					engine.trigger.apply(engine, args);
				};
			}(args));

			window.setTimeout(trigger, 16);
		};

		engine.BindingsReady = function () {
			engine._OnReady();
		};
	}

    /// @function engine.createDeferred
    /// Create a new deferred object.
    /// Use this to create deferred / promises that can be used together with `engine.call`.
    /// @return a new deferred object
    /// @see @ref CustomizingPromises
	engine.createDeferred = (global.engineCreateDeferred === undefined) ?
        function () { return new Promise(); }
        : global.engineCreateDeferred;

	/// @function engine.call
	/// Call asynchronously a C++ handler and retrieve the result
	/// The C++ handler must have been registered with `Coherent::UI::View::BindCall`
	/// @param {String} name name of the C++ handler to be called
	/// @param ... any extra parameters to be passed to the C++ handler
	/// @return deferred object whose promise is resolved with the result of the C++ handler
	engine.call = function () {
		engine._RequestId++;
		var id = engine._RequestId;

		var deferred = engine.createDeferred();
		engine._ActiveRequests[id] = deferred;
		var messageArguments = Array.prototype.slice.call(arguments);
		messageArguments.splice(1, 0, id);

		engine.SendMessage.apply(this, messageArguments);
		return deferred;
	};

	engine._Result = function (requestId) {
		var deferred = engine._ActiveRequests[requestId];

		delete engine._ActiveRequests[requestId];

		var resultArguments = Array.prototype.slice.call(arguments);
		resultArguments.shift();
		deferred.resolve.apply(deferred, resultArguments);
	};

	engine._Errors = [ 'Success', 'ArgumentType', 'NoSuchMethod', 'NoResult' ];

	engine._ForEachError = function (errors, callback) {
		var length = errors.length;

		for (var i = 0; i < length; ++i) {
			callback(errors[i].first, errors[i].second);
		}
	};

	engine._MapErrors = function (errors) {
		var length = errors.length;

		for (var i = 0; i < length; ++i) {
			errors[i].first = engine._Errors[errors[i].first];
		}
	};

	engine._TriggerError = function (type, message) {
		engine.trigger('Error', type, message);
	};

	engine._OnError = function (requestId, errors) {
		engine._MapErrors(errors);

		if (requestId === 0) {
			engine._ForEachError(errors, engine._TriggerError);
		}
		else {
			var deferred = engine._ActiveRequests[requestId];

			delete engine._ActiveRequests[requestId];

			deferred.reject(errors);
		}
	};

	engine._Register = function (eventName) {
		var trigger = (function (name, engine) {
			return function () {
				var eventArguments = [name];
				eventArguments.push.apply(eventArguments, arguments);
				engine.TriggerEvent.apply(this, eventArguments);
			};
		}(eventName, engine));

		engine.on(eventName, trigger);
	};

	engine._OnReady = function () {
		engine._BindingsReady = true;
		if (engine._WindowLoaded) {
			engine.trigger('Ready');
		}
	};

	engine._OnWindowLoaded = function () {
		engine._WindowLoaded = true;
		if (engine._BindingsReady) {
			engine.trigger('Ready');
		}
	};

	if (hasOnLoad) {
		global.onload = (function (originalWindowLoaded) {
			return function () {
				if (originalWindowLoaded) {
					originalWindowLoaded();
				}
				engine._OnWindowLoaded();
			};
		}(global.onload));
	} else {
		engine._WindowLoaded = true;
	}

	engine.on('_Result', engine._Result, engine);
	engine.on('_Register', engine._Register, engine);
	engine.on('_OnReady', engine._OnReady, engine);
	engine.on('_OnError', engine._OnError, engine);

	engine.BindingsReady();

	return engine;
});
