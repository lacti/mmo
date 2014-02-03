using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Server.Actor.Controller;
using Server.Core.Util;

namespace Server.Actor
{
    public class BaseActor : Actor<Action>, IDisposable
    {
        private readonly Dictionary<Type, BaseController> _controllerMap = new Dictionary<Type, BaseController>();

        public int ActorId { get; set; }

        public void Initialize()
        {
            InitializeControllers();
        }

        protected virtual void InitializeControllers()
        {
        }

        public virtual void Register(Coroutine coro)
        {
            coro.AddEntry(OnTick);
            foreach (var controller in _controllerMap.Values)
                coro.AddEntry(controller.OnTick);
        }

        public virtual void Write(Dictionary<string, object> objectMap)
        {
            objectMap.Add("ActorId", ActorId);
            foreach (var controller in _controllerMap.Values)
                controller.Write(objectMap);
        }

        public virtual void Load(Dictionary<string, object> objectMap)
        {
            ActorId = (int)objectMap["ActorId"];
            foreach (var controller in _controllerMap.Values)
                controller.Load(objectMap);
        }

        public T Attach<T>() where T : BaseController
        {
            var controllerType = typeof (T);
            if (!_controllerMap.ContainsKey(controllerType))
            {
                // Controller 생성자에서 Attach되므로 다시 Map에 추가할 필요는 없음
                return (T)Activator.CreateInstance(controllerType, this);
            }
            return (T)_controllerMap[controllerType];
        }

        public void Attach(BaseController controller)
        {
            var controllerType = controller.GetType();
            if (_controllerMap.ContainsKey(controllerType))
                throw new InvalidOperationException("duplicated controller");
            _controllerMap.Add(controllerType, controller);
        }

        public bool Detach<T>() where T : BaseController
        {
            var controllerType = typeof (T);
            return _controllerMap.Remove(controllerType);
        }

        public void DetachAll()
        {
            _controllerMap.Clear();
        }

        public bool Has<T>() where T : BaseController
        {
            var controllerType = typeof (T);
            return _controllerMap.ContainsKey(controllerType);
        }

        public T Get<T>() where T : BaseController
        {
            var controllerType = typeof(T);
            return _controllerMap.ContainsKey(controllerType) ? (T) _controllerMap[controllerType] : default(T);
        }

        protected override void ProcessMessage(Action message)
        {
            message();
        }

        public virtual IEnumerable<int> OnTick()
        {
            yield break;
        }

        public void Dispose()
        {
            DetachAll();
        }
    }
}
