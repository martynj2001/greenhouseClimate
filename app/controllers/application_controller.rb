class ApplicationController < ActionController::Base
    
    protect_from_forgery with: :exception
    protect_from_forgery unless: -> {request.format.json?}

    @@pumpStatus = '|0|'
    
end
