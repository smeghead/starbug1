Event.observe(window, "load", function() {
    var selector = new Selector('canvas');
    selector.findElements().each(
        function(elem) {
            var cg = new html5jp.graph.circle(elem.id);
            if( ! cg ) { return; }
            eval('if (' + elem.id + '.length) cg.draw(' + elem.id + '.sortBy(function(x){return x[1] * -1;}));');
        }
    );
});
//  vim: set ts=4 sw=4 sts=4 expandtab:
