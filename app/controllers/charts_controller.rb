class ChartsController < ApplicationController
    def hour_temp_out
        render json: Measure.group_by_hour(:created_at, last: 24).average(:temp_out)
    end

    def hour_temp_in
        render json: Measure.group_by_hour(:created_at, last: 24).average(:temp_out)
    end

    def hour_humidity_out
        render json: Measure.group_by_hour(:created_at, last: 24).average(:humidity_out)
    end

    def hour_humidity_in
        render json: Measure.group_by_hour(:created_at, last: 24).average(:humidity_in)
    end

    def week_temp_out
        render json: Measure.group_by_week(:created_at, last: 1).average(:temp_out)
    end

    def week_temp_in
        render json: Measure.group_by_week(:created_at, last: 1).average(:temp_out)
    end

    def week_humidity_out
        render json: Measure.group_by_week(:created_at, last: 1).average(:humidity_out)
    end

    def week_humidity_in
        render json: Measure.group_by_week(:created_at, last: 1).average(:humidity_in)
    end

end