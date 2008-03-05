Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('edit_top_form', 'submit', 
        function(e) {
            var dedide = confirm('登録します。よろしいですか？');
            if (!dedide) Event.stop(e);
            return dedide;
        }
    );
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
