using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(SmartBubbleMachine.Startup))]
namespace SmartBubbleMachine
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
            app.MapSignalR();
        }
    }
}
